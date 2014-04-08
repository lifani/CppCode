/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.9
*************************************/
#include "visionDaemon.h"

#define LOG_TAG "VISION_DAEMON"

const unsigned int MAX_INT_TIMES = 5;

BEGAIN_MESSAGE_MAP(CVisionDaemon, CBaseVision)
	ON_COMMAND(HEART_BIT, &CVisionDaemon::ProcessHeartMsg)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionDaemon, CBaseVision)
	ON_TIMER(200000, &CVisionDaemon::Daemon)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionDaemon)

CVisionDaemon::CVisionDaemon(const char* ppname, const char* pname) 
: CBaseVision(ppname, pname)
{
	 pthread_mutex_init(&m_lock, NULL);
}

CVisionDaemon::~CVisionDaemon()
{
	pthread_mutex_destroy(&m_lock);
}

int CVisionDaemon::ActiveImp()
{
	LOGW("vision deamon actived. %s : %d\n", __FILE__, __LINE__);
	return 0;
}

int CVisionDaemon::DeactiveImp()
{
	return 0;
}

void CVisionDaemon::Daemon()
{
	pthread_mutex_lock(&m_lock);
	
	map<int, PROC_INFO*>::iterator itm = m_mapProcInfo.begin();
	for (; itm != m_mapProcInfo.end();)
	{
		PROC_INFO* pProcInfo = itm->second;
		if (NULL != pProcInfo)
		{
			pProcInfo->times++;
		}
		
		if (pProcInfo->times > MAX_UNRECV_TIMES)
		{
			LOGW("lost connection to %s.", pProcInfo->pname.c_str());
			
			// 重启进程
			if (-1 == Restart(pProcInfo))
			{
				++itm;
				LOGE("restart %s err.", pProcInfo->pname.c_str());
			}
			else
			{
				// 添加新pid
				m_mapProcInfo[pProcInfo->pid] = pProcInfo;
				
				// 删除map元素
				itm->second = NULL;
				
				m_mapProcInfo.erase(itm++);
				
				LOGW("restart %s success.", pProcInfo->pname.c_str());
			}
		}
		else
		{
			++itm;
		}
	}
	
	pthread_mutex_unlock(&m_lock);
}

void CVisionDaemon::ProcessHeartMsg(VISION_MSG* pMsg)
{
	if (NULL != pMsg)
	{
		pthread_mutex_lock(&m_lock);
		
		pid_t pid = pMsg->data.size;
		
		PROC_INFO* pProcInfo = m_mapProcInfo[pid];
		if (NULL != pProcInfo)
		{
			if (pProcInfo->times > 0)
			{
				--pProcInfo->times;
			}
		}
		
		pthread_mutex_unlock(&m_lock);
	}
}

int CVisionDaemon::Restart(PROC_INFO* pProcInfo)
{
	if (NULL == pProcInfo)
	{
		return -1;
	}
	
	// 强杀进程
	KillProc(pProcInfo->pid);

	// 重启进程
	pid_t pid = 0;
	if (-1 == (pid = fork()))
	{
		return -1;
	}
	else if (pid > 0)
	{
		pProcInfo->pid = pid;
		pProcInfo->times = 0;
	}
	else if (0 == pid)
	{
		string name = pProcInfo->pname;
		
		// 替换进程内容
		if (-1 == execl(name.c_str(), name.c_str(), m_pname.c_str(), (char*)0))
		{
			LOGE("%s exec error. %s : %d\n", name.c_str(), __FILE__, __LINE__);
		}
	}
	
	return 0;
}

void CVisionDaemon::KillProc(pid_t pid)
{
	char szBuf[256] = {0};
	snprintf(szBuf, sizeof(szBuf), "kill -9 %d", pid);
	
	FILE* pf = popen(szBuf, "r");
	if (NULL != pf)
	{
		pclose(pf);
		pf = NULL;
	}
	
	waitpid(pid, 0, WCONTINUED);
}

