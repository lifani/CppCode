/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.4.4
*************************************/
#include "visionBm.h"

#define LOG_TAG "VISION_BM"

BEGAIN_MESSAGE_MAP(CVisionBm, CBaseVision)
	ON_COMMAND(BM_ID, &CVisionBm::ProcessMsg)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionBm, CBaseVision)
	ON_TIMER(200000, false, &CBaseVision::SendHeartMsg)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionBm)

CVisionBm::CVisionBm(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
{
}

CVisionBm::~CVisionBm()
{
}

int CVisionBm::ActiveImp()
{
	LOGW("vision bm actived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

int CVisionBm::DeactiveImp()
{
	return 0;
}

void CVisionBm::ProcessMsg(VISION_MSG* pMsg)
{
	if (NULL != pMsg)
	{
		unsigned int cnt0 = *(unsigned int*)(pMsg->data.ptr);
		unsigned int cnt1 = *(unsigned int*)(pMsg->data.ptr + MAX_CLOUD_SIZE);
	}
}
