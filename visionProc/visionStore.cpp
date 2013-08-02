#include "visionStore.h"
#include "tools.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready = PTHREAD_COND_INITIALIZER;

static StoreVision* QHead = NULL;
static StoreVision* QTail = NULL;

static char szData[512] = {0};

static unsigned int uDirIndex = 0;
static unsigned int uFileIndex = 0;

static int imu_fd = 0;

static bool store_vision_running = true;

bool InitStore()
{	
	if (!CreateOutDir())
	{
		return false;
	}
	
	return true;
}

void* store_vision(void* arg)
{
	StoreVision* p = NULL;
	
	unsigned int cnt = 0;
	while (store_vision_running)
	{
		pthread_mutex_lock(&lock);
		while (NULL == QHead)
		{
			pthread_cond_wait(&ready, &lock);
		}
		
		p 		= QHead;
		QHead 	= QHead->next;
		p->next = NULL;
		
		pthread_mutex_unlock(&lock);
		
		// 调用输出函数
		OutFile(p);
		
		printf("Receive %d frame img\n", cnt++);
		
		// 清除
		delete p;
		p = NULL;
	}
	
	close(imu_fd);
	
	return NULL;
}

void append_vision_queue(VisionNode*& pNode)
{
	if (NULL == pNode)
	{
		return;
	}
	
	StoreVision* p = new StoreVision;
	if (NULL == p)
	{
		return;
	}
	
	p->lImage = pNode->lImage;
	p->rImage = pNode->rImage;
	
	p->lLen = (int)pNode->lLen;
	p->rLen = (int)pNode->rLen;
	
	memcpy(&p->imu, &pNode->imu, sizeof(IMU));
	
	p->next = NULL;
	
	pNode->lImage = NULL;
	pNode->rImage = NULL;
	
	pthread_mutex_lock(&lock);
	
	if (NULL == QHead)
	{
		QHead = p;
		QTail = QHead->next;
	}
	else
	{
		QTail = p;
		QTail = QTail->next;
	}
	
	pthread_mutex_unlock(&lock);
	pthread_cond_signal(&ready);
}


void OutFile(StoreVision* p)
{
	sprintf(szData, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
		p->imu.acc_x, p->imu.acc_y, p->imu.acc_z,
		p->imu.gyro_x, p->imu.gyro_y, p->imu.gyro_z,
		p->imu.press,
		p->imu.q0, p->imu.q1, p->imu.q2, p->imu.q3);
	
	write(imu_fd, szData, strlen(szData));
	
	sprintf(szData, "./%d/%06d_l.dat\0", uDirIndex, uFileIndex);
	OutImg(p->lImage, p->lLen, szData);
	
	sprintf(szData, "./%d/%06d_r.dat\0", uDirIndex, uFileIndex++);
	OutImg(p->rImage, p->rLen, szData);
}

bool CreateOutDir()
{
	char szPath[10] = {0};
	for (uDirIndex = 0; uDirIndex < 10; ++uDirIndex)
	{
		sprintf(szPath, "./%d\0", uDirIndex);
		if (0 != access(szPath, F_OK))
		{
			break;
		}
	}
	
	if (10 == uDirIndex || 0 != mkdir(szPath, S_IRWXU | S_IRWXG | S_IRWXO))
	{
		return false;
	}
	
	sprintf(szPath, "./%d/imu.txt", uDirIndex);
	imu_fd = open(szPath, O_WRONLY | O_CREAT, 0666);
	if (-1 == imu_fd)
	{
		return false;
	}
	
	return true;
}

bool OutImg(const char* pData, int size, const char* szPath)
{
	if (NULL == pData || NULL == szPath)
	{
		return false;
	}
	
	int fd = open(szPath, O_WRONLY | O_CREAT, 0666);
	if (-1 == fd)
	{
		return false;
	}
	
	// write data
	write(fd, pData, size);
	
	close(fd);
	
	return true;
}

void exit_vision_store()
{
	store_vision_running =  false;
}

