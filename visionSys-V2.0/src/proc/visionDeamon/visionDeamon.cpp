/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.9
*************************************/
#include "visionDeamon.h"

#define LOG_TAG "VISION_DEAMON"

const unsigned int MAX_INT_TIMES = 5;

BEGAIN_TIMER_MAP(CVisionDeamon, CBaseVision)
	ON_TIMER(3000000, &CVisionDeamon::deamon)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionDeamon)

CVisionDeamon::CVisionDeamon(const char* ppname, const char* pname) 
: CBaseVision(ppname, pname)
, m_index(0)
, m_cnt(0)
{
}

CVisionDeamon::~CVisionDeamon()
{
}

int CVisionDeamon::ActiveImp()
{
	return 0;
}

int CVisionDeamon::DeactiveImp()
{
	return 0;
}

void CVisionDeamon::deamon()
{
	VISION_MSG msg;
	
	msg.id = DEAMON_ID;
	msg.data.ptr = (char*)&m_tDeamon;
	msg.data.size = sizeof(SHM_DEAMON);
	
	RecvMsg(&msg);
	
	if (m_index == m_tDeamon.index)
	{
		++m_cnt;
	}
	else
	{
		m_cnt = 0;
		m_index = m_tDeamon.index;
	}
	
	if (MAX_INT_TIMES == m_cnt)
	{
		LOGW("Error times reach max error times. %s : %d\n", __FILE__, __LINE__);
		
		// 去激活系统软件
		KillAll();
		
		CBaseVision::Deactive();
	}
}

void CVisionDeamon::KillAll()
{
	vector<PROC_CONFIG>::iterator itv = m_vProcConfig.begin();
	for (; itv != m_vProcConfig.end(); ++itv)
	{
		if (itv->name.compare(m_pname) != 0)
		{
			KillProc(itv->name);
		}
		
		vector<PROC_CONFIG>::iterator it = itv->vProcConfig.begin();
		for (; it != itv->vProcConfig.end(); ++it)
		{
			if (it->name.compare(m_pname) != 0)
			{
				KillProc(it->name);
			}
		}
	}
}

void CVisionDeamon::KillProc(string& pname)
{
	char szBuf[256] = {0};
	char szCmd[256] = {0};
	
	snprintf(szCmd, 256, "pidof %s", pname.c_str());
	
	FILE* pf = popen(szCmd, "r");
	if (NULL != pf)
	{
		if (fgets(szBuf, 255, pf) != NULL)
		{
			pid_t pid = atoi(szBuf);
			
			snprintf(szCmd, 256, "kill -9 %d", pid);
			
			FILE* p = popen(szCmd, "r");
			if (NULL != p)
			{
				pclose(p);
				p = NULL;
			}
		}
		
		pclose(pf);
		pf = NULL;
	}
}
