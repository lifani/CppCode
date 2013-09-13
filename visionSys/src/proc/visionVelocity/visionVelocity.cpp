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
	
	return 0;
}

void CVisionVelocity::Run()
{
	VELOCITY_DATA tVelocity;
	FEEDBACK_DATA tFeedback;
	
	unsigned int size = 0;
	
	while (m_bRunning)
	{
		if (-1 == RecvData(m_pname, (char*)&tVelocity, &size))
		{
			cout << "recv data error" << endl;
			
			continue;
		}
		
		cout << m_pname << " : " << (char*)&tVelocity << endl;
		
		// 算法接口
		
		// 数据反馈
		memcpy((char*)&tVelocity, (char*)tFeedback, size);
		
		sleep(2);
	}
}

int CVisionVelocity::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}