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
, m_key(0)
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
	
	string strAbsName = "";
	
	// 注册通信模块
	vector<PROC_INFO>::iterator itr = m_vProcInfo.begin();
	for (; itr != m_vProcInfo.end(); ++itr)
	{
		strAbsName = m_strCwd + string("/") + itr->pname;
		
		cout << "path = " << strAbsName << endl;
		cout << "pid = " << itr->pid << endl;
		key_t key = CMt::mt_init(SHM_MODE, strAbsName.c_str(), itr->pid, sizeof(SHM_DATA));
		if (-1 == key)
		{
			return -1;
		}
		
		m_mapShmKey[itr->pname] = key;
		m_key = key;
		
		cout << "m_key = " << m_key << endl;
	}
	
	return 0;
}

void CVisionRcm::Run()
{
	while (m_bRunning)
	{
		SHM_DATA shm_data;
		sprintf(shm_data.data, "%s\0", m_pname.c_str());
		
		if (CMt::mt_send(m_key, (char*)&shm_data, sizeof(SHM_DATA)) == -1)
		{
			cout << "send error." << endl;
		}
		
		sleep(2);
	}
}

int CVisionRcm::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}