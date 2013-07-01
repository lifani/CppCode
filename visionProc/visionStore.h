#ifndef _VISION_STORE_H_
#define _VISION_STORE_H_

#include "visionNode.h"

typedef struct tStoreVision
{
	char* lImage;
	char* rImage;
	IMU imu;
	
	struct tStoreVision* next;
	
	tStoreVision() : lImage(NULL), rImage(NULL), imu(), next(NULL)
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
		
		next = NULL;
	}
} StoreVision;

bool InitStore();

void* store_vision(void* arg);

void append_vision_queue(VisionNode*& pNode);

static void OutFile(StoreVision* p);

static bool CreateOutDir();

#endif
