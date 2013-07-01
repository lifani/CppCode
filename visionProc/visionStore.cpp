#include "visionStore.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready = PTHREAD_COND_INITIALIZER;

static StoreVision* QHead = NULL;
static StoreVision* QTail = NULL;

static unsigned char IMG_HEADER[54] = 
{
	0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
	54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 8, 0, 
	0, 0, 0, 0, 0x00, 0x2c, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static unsigned int uDirIndex = 0;
static unsigned int uFileIndex = 0;

static int imu_fd = 0;

bool InitStore()
{
	long lSize = IMG_SIZE + 54;
	IMG_HEADER[2] = (unsigned char)(lSize &0x000000ff);
    IMG_HEADER[3] = (lSize >> 8) & 0x000000ff;
    IMG_HEADER[4] = (lSize >> 16) & 0x000000ff;
    IMG_HEADER[5] = (lSize >> 24) & 0x000000ff;
	
	long width = IMG_WIDTH;
	IMG_HEADER[18] = width & 0x000000ff;
    IMG_HEADER[19] = (width >> 8) &0x000000ff;
    IMG_HEADER[20] = (width >> 16) &0x000000ff;
    IMG_HEADER[21] = (width >> 24) &0x000000ff;
	
	long height = IMG_HEIGHT;
	IMG_HEADER[22] = height &0x000000ff;
    IMG_HEADER[23] = (height >> 8) &0x000000ff;
    IMG_HEADER[24] = (height >> 16) &0x000000ff;
    IMG_HEADER[25] = (height >> 24) &0x000000ff;
	
	if (!CreateOutDir())
	{
		return false;
	}
	
	return true;
}

void* store_vision(void* arg)
{
	StoreVision* p = NULL;
	
	for (;;)
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
		
		// 清除
		delete p;
		p = NULL;
	}
	
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
}

void OutFile(StoreVision* p)
{
	char szData[256] = {0};
	sprintf(szData, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
		p->imu.acc_x, p->imu.acc_y, p->imu.acc_z,
		p->imu.gyro_x, p->imu.gyro_y, p->imu.gyro_z,
		p->imu.press,
		p->imu.q0, p->imu.q1, p->imu.q2, p->imu.q3);
	
	write(imu_fd, szData, strlen(szData));
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
	imu_fd = open(szPath, O_WRONLY);
	if (-1 == imu_fd)
	{
		return false;
	}
	
	return true;
}
