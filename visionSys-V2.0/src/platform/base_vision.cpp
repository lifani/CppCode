/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <platform/base_vision.h>
#include <mt/mt.h>
#include <sys/wait.h>
#include "monitor.h"

const VISION_MSGMAP* CBaseVision::GetMessageMap()
{
	return GetThisMessageMap();
}

const VISION_MSGMAP* CBaseVision::GetThisMessageMap()
{
	static const VISION_MSGMAP_ENTRY _messageEntries[] = 
	{
		{0, (VISION_PMSG)0}
	};
	
	static const VISION_MSGMAP messageMap = 
	{
		0, 
		&_messageEntries[0]
	};
	
	return &messageMap;
}

const VISION_TIMERMAP* CBaseVision::GetTimerMap()
{
	return GetThisTimerMap();
}

const VISION_TIMERMAP* CBaseVision::GetThisTimerMap()
{
	static const VISION_TIMERMAP_ENTRY _timerEntries[] = 
	{
		{0, (VISION_PTIMER)0}
	};
	
	static const VISION_TIMERMAP timerMap = 
	{
		0, 
		&_timerEntries[0]
	};
	
	return &timerMap;
}

CBaseVision::CBaseVision(const char* ppname, const char* pname)
: m_pname(pname)
, m_pid(0)
, m_ppname(ppname)
, m_ppid(0)
, m_strCwd("")
, m_bRunning(true)
, m_bTimerRunning(true)
{
}

CBaseVision::~CBaseVision()
{
}

/************************************
功能：	注册通信模块和消息分发线程
参数：	无
返回：	成功 0，失败 -1
************************************/
int CBaseVision::Active()
{
	// 获取进程信息：父进程ID，进程ID，运行路径
	GetProcInfo();
	
	// 注册通信模块
	string strAbsName = "";
	if (m_vProcInfo.size() > 0)
	{
		strAbsName = m_strCwd + string("/") + m_pname;
		if (CMT::Register(m_vMsgConfig, strAbsName, m_pid) == -1)
		{
			// log
			return -1;
		}
	}
	
	if (!m_ppname.empty())
	{
		strAbsName = m_strCwd + string("/") + m_ppname;
		if (CMT::Register(m_vMsgConfig, strAbsName, m_ppid) == -1)
		{
			// log
			return -1;
		}
	}
	
	// 注册线程函数
	RegisterThread(&CBaseVision::HandleMessage);
	
	return ActiveImp();
}

/************************************
功能：	启动线程和定时器
参数：	无
返回：	成功 0，失败 -1
************************************/
int CBaseVision::Action()
{
	vector<THREAD_ENTRY>::iterator itr = m_vThreadEntry.begin();
	for (; itr != m_vThreadEntry.end(); ++itr)
	{
		if (pthread_create(&itr->tid, NULL, StartThread, (void*)&(*itr)) != 0)
		{
			// log
			return -1;
		}
	}
	
	// Set Timer
	SetTimer();
	
	return 0;
}

/************************************
功能：	实现各种资源的释放
参数：	无
返回：	成功 0，失败 -1
************************************/
int CBaseVision::Deactive()
{
	// 停止定时器
	KillTimer();

	// 停止线程
	StopRun();
	
	if (DeactiveImp() == -1)
	{
		// log
		return -1;
	}
	
	// Deactive son proc
	vector<PROC_INFO>::iterator itv = m_vProcInfo.begin();
	for (; itv != m_vProcInfo.end(); ++itv)
	{
		char szBuf[256] = {0};
		sprintf(szBuf, "kill -2 %d", itv->pid);
		
		FILE* pf = popen(szBuf, "r");
		if (NULL != pf)
		{
			pclose(pf);
			pf = NULL;
		}
	}
	
	CMT::Destory();
	
	// 通知主线程退出
	NotifyExit(PROC_EXIT);
	
	return 0;
}

// 消息配置信息
void CBaseVision::AddConfig(vector<MSG_CONFIG>& vMsgXmlNode, vector<PROC_CONFIG>& vProcXmlNode)
{	
	m_vMsgConfig = vMsgXmlNode;
	m_vProcConfig = vProcXmlNode;
}

// 子进程信息
void CBaseVision::AddProcInfo(const char* pname, int pid)
{
	PROC_INFO tProcInfo;
	tProcInfo.pname = pname;
	tProcInfo.pid = pid;
	
	m_vProcInfo.push_back(tProcInfo);
}

// 选项信息
void CBaseVision::AddOption(vector<OPTION>& vOption)
{
	m_vOption = vOption;
}

/************************************
功能：	消息分发线程工作函数
参数：	无
返回：	无
************************************/
void CBaseVision::HandleMessage()
{
	while (IsRunning())
	{
		const VISION_MSGMAP* pMessageMap = GetMessageMap();
		for ( /* pMessageMap already init'ed */; pMessageMap->pfnGetBaseMap != NULL; 
			pMessageMap = (*pMessageMap->pfnGetBaseMap)())
		{
			const VISION_MSGMAP_ENTRY* pEntry = pMessageMap->pEntries;			
			for (; 0 != pEntry && 0 != pEntry->id && 0 != pEntry->pfn; pEntry++)
			{
				PostMessage(pEntry);
			}
		}
		
		USleep(5000);
	}
}

/************************************
功能：	检测是否有消息到达, 有则调用相应处理函数
参数：	pEntry const VISION_MSGMAP_ENTRY* 消息
返回：	无
************************************/
void CBaseVision::PostMessage(const VISION_MSGMAP_ENTRY* pEntry)
{
	if (NULL != pEntry)
	{
		VISION_MSG msg;
		msg.id = pEntry->id;
		
		if (0 == RecvMsg(&msg))
		{
			(this->*pEntry->pfn)(&msg);
		}
	}
}

int CBaseVision::RecvMsg(VISION_MSG* pMsg)
{
	return CMT::RecvMsg(pMsg);
}

int CBaseVision::SendMsg(VISION_MSG* pMsg)
{
	return CMT::SendMsg(pMsg);
}

void CBaseVision::RegisterThread(THREAD_FUNC pfn)
{
	THREAD_ENTRY entry;
	entry.pBaseVision = this;
	entry.pfn = pfn;
	
	m_vThreadEntry.push_back(entry);
}

bool CBaseVision::IsRunning()
{
	return m_bRunning;
}

void CBaseVision::StopRun()
{
	m_bRunning = false;
}

bool CBaseVision::IsTimerRunning()
{
	return m_bTimerRunning;
}

void CBaseVision::StopTimer()
{
	m_bTimerRunning = false;
}

void* CBaseVision::StartThread(void* arg)
{
	if (NULL != arg)
	{
		THREAD_ENTRY* pEntry = (THREAD_ENTRY*)arg;
		(pEntry->pBaseVision->*(pEntry->pfn))();
	}
	
	return NULL;
}

/************************************
功能：	定时器信号线程启动函数
参数：	arg void* 
返回：	NULL
************************************/
void* CBaseVision::StartReqTimer(void* arg)
{
	if (NULL != arg)
	{
		VISION_TIMER* pTimer = (VISION_TIMER*)arg;
		(pTimer->pBaseVision->ReqTimer)(pTimer);
	}
	
	return NULL;
}

/************************************
功能：	定时器响应线程启动函数
参数：	arg void* 
返回：	NULL
************************************/
void* CBaseVision::StartResTimer(void* arg)
{
	if (NULL != arg)
	{
		VISION_TIMER* pTimer = (VISION_TIMER*)arg;
		(pTimer->pBaseVision->ResTimer)(pTimer);
	}
	
	return NULL;
}

void CBaseVision::AddLog(const string& strLog)
{
	if (!strLog.empty())
	{
		m_vLog.push_back(strLog);
	}
}

void CBaseVision::OutLog(VISION_MSG* pMsg)
{
	// log
	vector<string>::iterator itv = m_vLog.begin();
	for (; itv != m_vLog.end(); ++itv)
	{
		cout << *itv << endl;
	}
}

/************************************
功能：	定时器函数，定时生成信号
参数：	arg void* 
返回：	NULL
************************************/
void CBaseVision::ReqTimer(VISION_TIMER* pTimer)
{
	while (IsTimerRunning())
	{
		// 定时
		USleep(pTimer->timeusec);
		
		// 发送信号
		pthread_kill(pTimer->res_tid, pTimer->signo);
	}
}

/************************************
功能：	定时器函数，响应信号
参数：	arg void* 
返回：	NULL
************************************/
void CBaseVision::ResTimer(VISION_TIMER* pTimer)
{
	int err = 0;
	int signo = 0;
	
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, pTimer->signo);
	
	while (IsTimerRunning())
	{
		if ((err = sigwait(&mask, &signo)) != 0)
		{
			// log
			cout << "err : " << err << endl;
		}
		
		if (signo == pTimer->signo)
		{
			(this->*pTimer->pfn)();
		}
		else if (signo == SIGQUIT)
		{
			// log
			cout << "recv SIGQUIT" << endl;
		}
		else
		{
			// log
		}
	}
}

/************************************
功能：	启动定时器
参数：	无
返回：	无
************************************/
void CBaseVision::SetTimer()
{
	// timer stuct point
	VISION_TIMER* pTimer;
	// 信号
	int signo = SIGUSR1 + 1;
	
	const VISION_TIMERMAP* pTimerMap = GetTimerMap();
	for ( /* pTimerMap already init'ed */; NULL != pTimerMap->pfnGetBaseTimerMap; 
		pTimerMap = (*pTimerMap->pfnGetBaseTimerMap)())
	{
		const VISION_TIMERMAP_ENTRY* pEntry = pTimerMap->pEntries;
		for (; 0 != pEntry && 0 != pEntry->timeusec && 0 != pEntry->pfn; pEntry++)
		{
			pTimer = new VISION_TIMER;
			
			pTimer->pBaseVision = this;
			pTimer->timeusec = pEntry->timeusec;
			pTimer->signo = signo++;
			pTimer->pfn = pEntry->pfn;
			
			// 首先启动定时器响应线程
			if (pthread_create(&(pTimer->res_tid), NULL, StartResTimer, (void*)pTimer) != 0)
			{
				// log
				return ;
			}
			
			// 再启动定时器请求线程
			if (pthread_create(&(pTimer->req_tid), NULL, StartReqTimer, (void*)pTimer) != 0)
			{
				// log
				return ;
			}
			
			m_vTimer.push_back(pTimer);
		}
	}
}

/************************************
功能：	停止定时器
参数：	无 
返回：	无
************************************/
void CBaseVision::KillTimer()
{
	StopTimer();
	
	vector<VISION_TIMER*>::iterator itv = m_vTimer.begin();
	for (; itv != m_vTimer.end(); ++itv)
	{
		pthread_kill((*itv)->res_tid, SIGQUIT);
		
		delete *itv;
		*itv = NULL;
	}
	
	m_vTimer.clear();
}

void CBaseVision::GetProcInfo()
{
	GetProcInfoImp(m_vProcConfig);
	
	char szPath[256] = {0};
	getcwd(szPath, sizeof(szPath));
	
	m_strCwd = string(szPath);
}

void CBaseVision::GetProcInfoImp(vector<PROC_CONFIG>& vProcConfig)
{
	vector<PROC_CONFIG>::iterator it = vProcConfig.begin();
	for (; it != vProcConfig.end(); ++it)
	{
		if (m_pname.compare(it->name) == 0)
		{
			m_pid = it->pid;
			m_ppid = it->ppid;
			
			return;
		}
		else
		{
			GetProcInfoImp(it->vProcConfig);
		}
	}
	
	return;
}

void CBaseVision::USleep(unsigned int usec)
{
	struct timeval delay;
	
	delay.tv_sec = 0;
	delay.tv_usec = usec;
	
	select(0, NULL, NULL, NULL, &delay);
}

