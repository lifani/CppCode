#include <common_interface.h>
#include <mt/mt.h>
#include "visionVelocity.h"

CCommonInterface* CreateInstance(const char* ppname, const char* pname)
{
	if (NULL == ppname || NULL == pname)
	{
		return NULL;
	}
	
	CCommonInterface* p = new CVisionVelocity(ppname, pname);
	
	return p;
}

CVisionVelocity::CVisionVelocity(const char* ppname, const char* pname)
: CBaseVision(ppname, pname), m_bRunning(true)
{
}

CVisionVelocity::~CVisionVelocity()
{
}

int CVisionVelocity::Active()
{
	if (CBaseVision::Active() == -1)
	{
		return -1;
	}
	
	m_bRunning = true;
	
	if (m_VisionStore.Init(VELOCITY_PATH))
	{
		return -1;
	}
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	
	return 0;
}

void CVisionVelocity::Run()
{
	VELOCITY_DATA tVelocity;
	FEEDBACK_DATA tFeedback;
	
	unsigned int size = 0;
	
	while (m_bRunning)
	{
		int len = 0;
		if (-1 == (len = RecvData(m_pname, (char*)&tVelocity, &size)))
		{
			cout << "recv data error" << endl;
			
			continue;
		}
		
		if (len == 0)
		{
			continue;
		}
		
		// 算法接口
		
		// 存入存储模块
		m_VisionStore.push(tVelocity.lCloud, tVelocity.lcnt * POINT_LEN, tVelocity.rCloud, tVelocity.rcnt * POINT_LEN);
		
		// 数据反馈
		memcpy((char*)&tVelocity, (char*)&tFeedback, size);
		
	}
}

void CVisionVelocity::Run1()
{
	while(m_bRunning)
	{
		m_VisionStore.pop();
	}
}

int CVisionVelocity::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}