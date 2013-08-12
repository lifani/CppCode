#ifndef VISIONNODE_H_INCLUDED
#define VISIONNODE_H_INCLUDED

#include "typedef.h"

class VisionNode
{
public:

	static VisionNode* Instance();
	
	static bool InitMemory();

	static void destroy(VisionNode* p)
    {
		p->~VisionNode();
    }

private:

	VisionNode();

	~VisionNode();

public:

	char* lImage;
	char* rImage;
	
	unsigned short lLen;
	unsigned short rLen;
	
	IMU imu;

	VisionNode* next;

	static int index;
	static char* M_BUF;
};


#endif // VISIONNODE_H_INCLUDED
