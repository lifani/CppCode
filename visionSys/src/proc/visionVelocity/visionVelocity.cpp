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
	
	string strAbsName = m_strCwd + string("/") + m_pname;
		
	// ×¢²áÍ¨ÐÅÄ£¿é
	if ((m_shmkey = CMt::mt_init(SHM_MODE, strAbsName.c_str(), m_pid, sizeof(SHM_DATA))) == -1)
	{
		return -1;
	}
	
	cout << "m_shmkey = " << m_shmkey << endl;
	
	m_bRunning = true;
	
	return 0;
}

void CVisionVelocity::Run()
{
	while (m_bRunning)
	{
		SHM_DATA shm_data;
		
		if (-1 == CMt::mt_recv(m_shmkey, (char*)&shm_data, sizeof(SHM_DATA)))
		{
			sleep(50);
			cout << "recv data error" << endl;
		}
		
		cout << shm_data.data << endl;
	}
}

int CVisionVelocity::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}