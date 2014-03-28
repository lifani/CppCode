/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.9
*************************************/
#include "visionDeamon.h"

#define LOG_TAG "VISION_DEAMON"

const unsigned int MAX_INT_TIMES = 5;

BEGAIN_TIMER_MAP(CVisionDeamon, CBaseVision)
	//ON_TIMER(3000000, &CVisionDeamon::deamon)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionDeamon)

CVisionDeamon::CVisionDeamon(const char* ppname, const char* pname) 
: CBaseVision(ppname, pname)
{
}

CVisionDeamon::~CVisionDeamon()
{
}

int CVisionDeamon::ActiveImp()
{
	LOGW("vision deamon actived. %s : %d\n", __FILE__, __LINE__);
	return 0;
}

int CVisionDeamon::DeactiveImp()
{
	return 0;
}
