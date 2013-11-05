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

CBaseVision::CBaseVision(const char* ppname, const char* pname)
: m_ppname(ppname)
, m_pname(pname)
, m_ppid(0)
, m_pid(0)
, m_strCwd("")
, m_bRunning(true)
{
}

CBaseVision::~CBaseVision()
{
}

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

int CBaseVision::Action()
{
	vector<THREAD_ENTRY>::iterator itr = m_vThreadEntry.begin();
	for (; itr != m_vThreadEntry.end(); ++itr)
	{
		if (pthread_create(&itr->tid, NULL, StartThread, (void*)&(*itr)))
		{
			// log
			return -1;
		}
	}
	
	return 0;
}

int CBaseVision::Deactive()
{
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

void CBaseVision::HandleMessage()
{
	while (IsRunning())
	{
		const VISION_MSGMAP* pMessageMap = GetMessageMap();
		for ( /* pMessageMap already init'ed */; pMessageMap->pfnGetBaseMap != NULL; 
			pMessageMap = (*pMessageMap->pfnGetBaseMap)())
		{
			const VISION_MSGMAP_ENTRY* pEntry = pMessageMap->pEntries;
			if (0 != pEntry && 0 != pEntry->id && 0 != pEntry->pfn)
			{
				PostMessage(pEntry);
			}
		}
		
		usleep(10000);
	}
}

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

void* CBaseVision::StartThread(void* arg)
{
	if (NULL != arg)
	{
		THREAD_ENTRY* pEntry = (THREAD_ENTRY*)arg;
		(pEntry->pBaseVision->*(pEntry->pfn))();
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

