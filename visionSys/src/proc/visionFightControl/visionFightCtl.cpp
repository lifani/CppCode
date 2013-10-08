#include <common_interface.h>
#include <mt/mt.h>
#include "visionFightCtl.h"

CCommonInterface* CreateInstance(const char* ppname, const char* pname)
{
	if (NULL == ppname || NULL == pname)
	{
		return NULL;
	}
	
	CCommonInterface* p = new CVisionFightCtl(ppname, pname);
	
	return p;
}

CVisionFightCtl::CVisionFightCtl(const char* ppname, const char* pname)
: CBaseVision(ppname, pname), m_bRunning(true)
{
}

CVisionFightCtl::~CVisionFightCtl()
{
}

int CVisionFightCtl::Active()
{
	if (-1 == CBaseVisioin::Active())
	{
		return -1;
	}
	
	m_bRunning = true;
	
	return 0;
}

void CVisionFightCtl::Run()
{
	FIGHTCTL_DATA tFightCtl;
	FEEDBACK_DATA tFeedback;
	
	unsigned int size = 0;
	
	while (m_bRunning)
	{
		int len = 0;
		if (-1 == (len = RecvData(m_pname, (char*)&tFightCtl, &size)))
		{
			cout << "recv data error." << endl;
			
			continue;
		}
		
		if (0 == len)
		{
			continue;
		}
		
		usleep(100);
	}
}

int CVisionFightCtl::Deactive()
{
	m_bRunning = false;
	
	CBaseVision::Deactive();
}
