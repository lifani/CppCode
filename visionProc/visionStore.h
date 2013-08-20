#ifndef _VISION_STORE_H_
#define _VISION_STORE_H_

#include "visionNode.h"

typedef struct tStoreVision
{
	char* lImage;
	char* rImage;
	
	unsigned int lLen;
	unsigned int rLen;
	
	unsigned int index;
	
	IMU imu;
	
	struct tStoreVision* next;
	
	tStoreVision() : lImage(NULL), rImage(NULL), lLen(0), rLen(0), index(0), imu(), next(NULL)
	{
	}
	
	~tStoreVision()
	{
		if (NULL != lImage)
		{
			delete []lImage;
			lImage = NULL;
		}
		
		if (NULL != rImage)
		{
			delete []rImage;
			rImage = NULL;
		}
		
		//cout << "delete " << index << endl;
		
		next = NULL;
	}
} StoreVision;

bool InitStore();

void* store_vision(void* arg);

void append_vision_queue(VisionNode*& pNode);

static void OutFile(StoreVision* p);

static bool CreateOutDir();

static bool OutImg(const char* pData, unsigned int size, const char* szPath);

void exit_vision_store();

#endif
