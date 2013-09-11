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
	while (m_bRunning)
	{
		SHM_DATA shm_data;
		sprintf(shm_data.data, "%s\0", m_pname.c_str());
		
		unsigned int size = strlen(shm_data.data);
		if (-1 == RecvData(m_pname, (char*)&shm_data, &size))
		{
			sleep(50);
			cout << "recv data error" << endl;
		}
		
		cout << m_pname << " : " << shm_data.data << endl;
		
		sleep(2);
	}
}

int CVisionVelocity::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}