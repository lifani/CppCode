/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.9
*************************************/
#include "visionDaemon.h"

#include "../../common/can_data/CHF.h"

#define LOG_TAG "VISION_DAEMON"

BEGAIN_MESSAGE_MAP(CVisionDaemon, CBaseVision)
	ON_COMMAND(HEART_BIT, &CVisionDaemon::ProcessHeartMsg)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionDaemon, CBaseVision)
	ON_TIMER(200000, false, &CVisionDaemon::Daemon)
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
	if (Initialize() < 0)
	{
		return 0;
	}

	LOGW("vision deamon actived. %s : %d\n", __FILE__, __LINE__);
	return 0;
}

int CVisionDaemon::DeactiveImp()
{
	//CHF::Destroy();

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
		
		if (pProcInfo->times > MAX_UNRECV_TIMES && pProcInfo->restart_times < MAX_RESTART_TIMES)
		{
			LOGW("lost connection to %s.", pProcInfo->pname.c_str());
			
			// 重启进程
			if (-1 == Restart(pProcInfo))
			{
				++itm;
				LOGE("restart %s err (%d).", pProcInfo->pname.c_str(), pProcInfo->restart_times);
			}
			else
			{
				// 添加新pid
				m_mapProcInfo[pProcInfo->pid] = pProcInfo;
				
				// 删除map元素
				itm->second = NULL;
				
				m_mapProcInfo.erase(itm++);
				
				LOGW("restart %s success (%d).", pProcInfo->pname.c_str(), pProcInfo->restart_times);
			}
		}
		else
		{
			if (pProcInfo->restart_times == MAX_RESTART_TIMES)
			{
				m_code = ERR_PROC_UNINTIED;
			}
			
			++itm;
		}
		
		// 取得最小错误码
		if (pProcInfo->code < m_code)
		{
			m_code = pProcInfo->code;
		}
	}
	
	pthread_mutex_unlock(&m_lock);
	
	// 上报状态
	//SendAlarm();
}

void CVisionDaemon::ProcessHeartMsg(VISION_MSG* pMsg)
{
	if (NULL != pMsg)
	{
		pthread_mutex_lock(&m_lock);
		
		// 心跳消息中size内容为pid
		pid_t pid = pMsg->data.x.pid;
		
		PROC_INFO* pProcInfo = m_mapProcInfo[pid];
		if (NULL != pProcInfo)
		{
			pProcInfo->code = pMsg->data.y.code;
			pProcInfo->times = 0;
			pProcInfo->restart_times = 0;
		}
		
		pthread_mutex_unlock(&m_lock);
	}
}

int CVisionDaemon::Initialize()
{
	/*m_can = CHF::FD(HF_CAN1);
	if (-1 == m_can)
	{
		LOGE("Get can interface err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (-1 == CHF::Initialize(false))
	{
		LOGE("CHF init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}*/
	
	return 0;
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
		pProcInfo->restart_times++;
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

void CVisionDaemon::SendAlarm()
{
	VISION_STATUS status;
	
	status.code = m_code;
	
	SendCanData(m_can, 0x609, (char*)&status, sizeof(VISION_STATUS));
}
