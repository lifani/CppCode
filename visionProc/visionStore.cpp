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

static const unsigned ARRAY_SIZE = 256;
static StoreVision* pStoreVision[ARRAY_SIZE]; 
static unsigned int uFetchPos = 0;
static unsigned int uStorePos = 0;

static bool store_vision_running = true;

bool InitStore()
{	
	if (!CreateOutDir())
	{
		return false;
	}
	
	memset((char*)pStoreVision, 0, ARRAY_SIZE * sizeof(StoreVision*));
	
	return true;
}

void* store_vision(void* arg)
{
	StoreVision* p = NULL;
	
	while (store_vision_running)
	{
		pthread_mutex_lock(&lock);
		while (NULL == pStoreVision[uFetchPos])
		{
			pthread_cond_wait(&ready, &lock);
		}
		
		p = pStoreVision[uFetchPos];
		pStoreVision[uFetchPos++] = NULL;
		
		if (uFetchPos == ARRAY_SIZE)
		{
			uFetchPos = 0;
		}
		
		pthread_mutex_unlock(&lock);
		
		// 调用输出函数
		OutFile(p);
		
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
	
	p->lLen = (unsigned int)pNode->lLen;
	p->rLen = (unsigned int)pNode->rLen;
	
	p->index = uFileIndex++;
	
	memcpy(&p->imu, &pNode->imu, sizeof(IMU));
	
	p->next = NULL;
	
	pNode->lImage = NULL;
	pNode->rImage = NULL;
	
	pthread_mutex_lock(&lock);
	
	if (NULL != pStoreVision[uStorePos])
	{
		delete pStoreVision[uStorePos];
	}
	
	pStoreVision[uStorePos++] = p;
	
	if (uStorePos == ARRAY_SIZE)
	{
		uStorePos = 0;
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
	
	sprintf(szData, "./%d/%06d_l.dat\0", uDirIndex, p->index);
	if (!OutImg(p->lImage, p->lLen * 36, szData))
	{
		cout << "out put left img fail." << endl;
	}
	
	sprintf(szData, "./%d/%06d_r.dat\0", uDirIndex, p->index);
	if (!OutImg(p->rImage, p->rLen * 36, szData))
	{
		cout << "out put right img fail." << endl;
	}
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

bool OutImg(const char* pData, unsigned int size, const char* szPath)
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
	
	//cout << "size = " << size << endl;
	
	return true;
}

void exit_vision_store()
{
	store_vision_running =  false;
}

