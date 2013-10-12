#include <common_interface.h>
#include <mt/mt.h>
#include <bm/main_bm.h>
#include "visionBm.h"

extern const int sizeOutBuf; 

CCommonInterface* CreateInstance(const char* ppname, const char* pname)
{
	if (NULL == ppname || NULL == pname)
	{
		return NULL;
	}
	
	CCommonInterface* p = new CVisionBm(ppname, pname);
	
	return p;
}

CVisionBm::CVisionBm(const char* ppname, const char* pname)
: CBaseVision(ppname, pname), m_bRunning(true)
{
}

CVisionBm::~CVisionBm()
{
}

int CVisionBm::Active()
{
	if (-1 == CBaseVision::Active())
	{
		LOGE("%s's basic class active error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
		return -1;
	}
	
	m_bRunning = true;
	
	InitBm();
	
	m_VisionStore.Init(RECTIFIED_PATH);
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	
	LOGW("%s active success. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
	
	return 0;
}

void CVisionBm::Run()
{
	RECTIFIED_IMG tRectified;
	FEEDBACK_DATA tFeedback;
	
	tFeedback.flg = IS_BM;
	
	unsigned int size = 0;
	unsigned int i = 0;

	while (m_bRunning)
	{
		int len = 0;
		if (-1 == (len = RecvData(m_pname, (char*)&tRectified, &size)))
		{
			usleep(100000);
			continue;
		}
		
		if (0 == len)
		{
			usleep(100000);
			continue;
		}
		
		// 算法接口 参数：tFeedback.data
		RunBm( (char*)tRectified.lImg, (char*)tRectified.rImg, (char*)tFeedback.data);
		
		// 存入存储模块
		m_VisionStore.push(tRectified.lImg, IMG_SIZE, tRectified.rImg, IMG_SIZE);

		tFeedback.cnt = i++;
		tFeedback.size = sizeOutBuf;
		
		size = sizeOutBuf + 3 * sizeof(int);
		// 数据反馈
		memcpy((char*)&tRectified, (char*)&tFeedback, size);
	}
}

void CVisionBm::Run1()
{
	while (m_bRunning)
	{
		m_VisionStore.pop();
	}
}

int CVisionBm::Deactive()
{
	m_bRunning = false;
	
	ReleaseBm();

	return CBaseVision::Deactive();
}
