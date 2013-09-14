#include <common_interface.h>
#include <mt/mt.h>
#include "visionBm.h"

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
		return -1;
	}
	
	m_bRunning = true;
	
	m_VisionStore.Init(RECTIFIED_PATH);
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	
	return 0;
}

void CVisionBm::Run()
{
	RECTIFIED_IMG tRectified;
	FEEDBACK_DATA tFeedback;
	
	unsigned int size = 0;
	
	while (m_bRunning)
	{
		int len = 0;
		if (-1 == (len = RecvData(m_pname, (char*)&tRectified, &size)))
		{
			cout << "recv data error." << endl;
			
			continue;
		}
		
		if (0 == len)
		{
			continue;
		}
		
		// 算法接口
		
		// 存入存储模块
		m_VisionStore.push(tRectified.lImg, IMG_SIZE, tRectified.rImg, IMG_SIZE);
		
		// 数据反馈
		memcpy((char*)&tRectified, (char*)&tFeedback, size);
		
		//sleep(50000);
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
	
	return CBaseVision::Deactive();
}
