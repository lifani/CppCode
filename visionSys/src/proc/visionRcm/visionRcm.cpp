#include <common_interface.h>
#include <mt/mt.h>
#include "visionRcm.h"

CCommonInterface* CreateInstance(const char* ppname, const char* pname)
{
	if (NULL == ppname || NULL == pname)
	{
		return NULL;
	}
	
	CCommonInterface* p = new CVisionRcm(ppname, pname);
	
	return p;
}

CVisionRcm::CVisionRcm(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, m_bRunning(true)
{
}

CVisionRcm::~CVisionRcm()
{
}

int CVisionRcm::Active()
{
	// 使用父类的Active
	if (CBaseVision::Active() == -1)
	{
		return -1;
	}
	
	m_bRunning = true;
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	
	return 0;
}

void CVisionRcm::Run()
{
	while (m_bRunning)
	{
		SHM_DATA shm_data;
		sprintf(shm_data.data, "%s\0", m_pname.c_str());
		
		unsigned int size = sizeof(SHM_DATA);
		if (SendData(string("visionVelocity"), (char*)&shm_data, &size) == -1)
		{
			cout << "send error." << endl;
		}
		
		cout << m_pname << " : " << shm_data.data << endl;
		
		sleep(2);
	}
}

void CVisionRcm::Run1()
{
	while(m_bRunning)
	{
		// 读取IMU数据
		
		sleep(2);
	}
}

int CVisionRcm::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}