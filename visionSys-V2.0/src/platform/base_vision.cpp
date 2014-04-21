/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <platform/base_vision.h>
#include <mt/mt.h>
#include <sys/wait.h>
#include "monitor.h"
#include "../common/can_data/CHF.h"

#define LOG_TAG "BASE_VISION"

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
		{0, false, (VISION_PTIMER)0}
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
, m_code(0)
, m_wTime(1)
, m_ppname(ppname)
, m_ppid(0)
, m_strCwd("")
, m_bRunning(true)
, m_bTimerRunning(true)
{
	pthread_mutex_init(&m_lock, NULL);
	pthread_cond_init(&m_ready, NULL);
	
	pthread_mutex_init(&m_errLock, NULL);
}

CBaseVision::~CBaseVision()
{
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_ready);
}

/************************************
���ܣ�	ע��ͨ��ģ�����Ϣ�ַ��߳�
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CBaseVision::Active()
{
	// ��ȡ������Ϣ��������ID������ID������·��
	GetProcInfo();
	
	// ����״̬��
	SetStatusCode(ERR_SYSTIME_OK);
	
	// ע��ͨ��ģ��
	string strAbsName = "";
	if (!m_ppname.empty())
	{
		strAbsName = m_strCwd + string("/") + m_ppname;
		if (CMT::Register(m_mapPMsgTag, strAbsName, m_ppid) == -1)
		{
			// log
			return -1;
		}
	}
	
	// ע���̺߳���
	RegisterThread(&CBaseVision::HandleMessage);
	
	// Set Timer
	SetTimer();
	
	// �������༤���
	
	if (-1 == ActiveImp())
	{
		SetStatusCode(ERR_PROC_UNINTIED);
		return -1;
	}
	
	// ע����Ϣ���ɺ���
	InitMsgFunc();
	
	return 0;
}

/************************************
���ܣ�	�����̺߳Ͷ�ʱ��
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
	
	return 0;
}

/************************************
���ܣ�	ʵ�ָ�����Դ���ͷ�
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CBaseVision::Deactive()
{
	// ֹͣ��ʱ��
	KillTimer();

	// ֹͣ�߳�
	StopRun();
	
	if (DeactiveImp() == -1)
	{
		// log
		return -1;
	}
	
	// ȥ�����ӽ���
	map<int, PROC_INFO*>::iterator itm = m_mapProcInfo.begin();
	for (; itm != m_mapProcInfo.end(); ++itm)
	{
		char szBuf[256] = {0};
		sprintf(szBuf, "kill -2 %d", itm->first);
		
		FILE* pf = popen(szBuf, "r");
		if (NULL != pf)
		{
			pclose(pf);
			pf = NULL;
		}
		
		if (NULL != itm->second)
		{
			delete itm->second;
			itm->second = NULL;
		}
	}
	
	CMT::Destory();
	
	// ֪ͨ���߳��˳�
	NotifyExit(PROC_EXIT);
	
	return 0;
}

// ��Ϣ������Ϣ
void CBaseVision::AddMsgTag(const map<long, MSG_TAG*>& mapMsgTag, const map<string, PROC_TAG>& mapProcTag)
{	
	m_mapPMsgTag = mapMsgTag;
	
	map<string, PROC_TAG>::const_iterator itm = mapProcTag.begin();
	for (; itm != mapProcTag.end(); ++itm)
	{
		if (itm->first.compare(m_pname) == 0)
		{
			m_procTag = itm->second;
			break;
		}
		
		vector<PROC_TAG>::const_iterator itv = itm->second.vProcTag.begin();
		for (; itv != itm->second.vProcTag.end(); ++itv)
		{
			if (itv->name.compare(m_pname) == 0)
			{
				m_procTag = *itv;
				
				vector<MSG_TAG*>::iterator itv = m_procTag.vPMsgTag.begin();
				for (; itv != m_procTag.vPMsgTag.end(); ++itv)
				{
					MSG_TAG* pMsgTag = *itv;
					while (NULL != pMsgTag)
					{
						pMsgTag = pMsgTag->next;
					}
				}

				return;
			}
		}
	}
}

// �ӽ�����Ϣ
void CBaseVision::AddProcInfo(const char* pname, int pid)
{
	PROC_INFO* pProcInfo = new PROC_INFO;
	
	pProcInfo->pname = pname;
	pProcInfo->pid = pid;
	pProcInfo->times = 0;
	pProcInfo->restart_times = 0;
	
	m_mapProcInfo[pid] = pProcInfo;
}

// ѡ����Ϣ
void CBaseVision::AddOption(vector<OPTION>& vOption)
{
	m_vOption = vOption;
}

/************************************
���ܣ�	��Ϣ�ַ��̹߳�������
������	��
���أ�	��
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
���ܣ�	����Ƿ�����Ϣ����, ���������Ӧ������
������	pEntry const VISION_MSGMAP_ENTRY* ��Ϣ
���أ�	��
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

int CBaseVision::SendSmallMsg(long id, char* ptr, unsigned int size)
{
	VISION_MSG msg;
	
	msg.id = id;
	msg.data.x.size = size;
	msg.data.ptr = NULL;
	
	memcpy(msg.data.y.buf, ptr, size);
	
	return CMT::SendMsg(&msg);
}

void CBaseVision::RegisterThread(THREAD_FUNC pfn)
{
	THREAD_ENTRY entry;
	entry.pBaseVision = this;
	entry.pfn = pfn;
	
	m_vThreadEntry.push_back(entry);
}

void CBaseVision::RegisterMsgFunc(string name, MSG_FUNC func)
{
	m_mapMsgFunc[name] = func;
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
���ܣ�	��ʱ���ź��߳���������
������	arg void* 
���أ�	NULL
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
���ܣ�	��ʱ����Ӧ�߳���������
������	arg void* 
���أ�	NULL
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
���ܣ�	��ʱ����������ʱ�����ź�
������	arg void* 
���أ�	NULL
************************************/
void CBaseVision::ReqTimer(VISION_TIMER* pTimer)
{
	if (pTimer->bWait)
	{
		pthread_mutex_lock(&m_lock);
		while(true)
		{
			pthread_cond_wait(&m_ready, &m_lock);
			break;
		}
		
		pthread_mutex_unlock(&m_lock);
		
		USleep(m_wTime);
	}

	unsigned int usec = pTimer->timeusec;
	unsigned int delay = 0;
	
	struct timeval last_val;
	struct timeval cur_val;
	
	gettimeofday(&last_val, NULL);
	
	while (IsTimerRunning())
	{
		// ��ʱ
		USleep(usec);
		
		// �����ź�
		pthread_kill(pTimer->res_tid, pTimer->signo);
		
		gettimeofday(&cur_val, NULL);
		
		delay = GetDelayTime(&last_val, &cur_val, pTimer->timeusec);
		
		usec = delay > 500 ? pTimer->timeusec - delay : pTimer->timeusec;
	}
}

/************************************
���ܣ�	��ʱ����������Ӧ�ź�
������	arg void* 
���أ�	NULL
************************************/
void CBaseVision::ResTimer(VISION_TIMER* pTimer)
{
	int err = 0;
	int signo = 0;
	
	sigset_t mask;
	sigset_t oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, pTimer->signo);
	
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
	{
		cout << "errno err = " << err << endl;
	}
	
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
���ܣ�	������ʱ��
������	��
���أ�	��
************************************/
void CBaseVision::SetTimer()
{
	// timer stuct point
	VISION_TIMER* pTimer;
	// �ź�
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
			pTimer->bWait = pEntry->bWait;
			
			// ����������ʱ����Ӧ�߳�
			if (pthread_create(&(pTimer->res_tid), NULL, StartResTimer, (void*)pTimer) != 0)
			{
				// log
				return ;
			}
			
			// ��������ʱ�������߳�
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
���ܣ�	ֹͣ��ʱ��
������	�� 
���أ�	��
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

/************************************
���ܣ�	����������Ϣ
������	�� 
���أ�	��
************************************/
void CBaseVision::SendHeartMsg()
{
	VISION_MSG msg;
	
	msg.id = HEART_BIT;
	msg.data.ptr = NULL;
	msg.data.x.pid = m_pid;
	msg.data.y.code = m_code;
	
	if (-1 == CMT::SendMsg(&msg))
	{
		LOGE("proc %d send msg err.", m_pid);
	}
}

void CBaseVision::GetProcInfo()
{
	//m_pid = m_procTag.pid;
	m_pid = getpid();
	m_ppid = m_procTag.ppid;
	
	char szPath[256] = {0};
	getcwd(szPath, sizeof(szPath));
	
	m_strCwd = string(szPath);
}

void CBaseVision::InitMsgFunc()
{
	vector<MSG_TAG*>::iterator itv = m_procTag.vPMsgTag.begin();
	for (; itv != m_procTag.vPMsgTag.end(); ++itv)
	{
		MSG_TAG* pMsgTag = *itv;
		
		while (NULL != pMsgTag)
		{
			pMsgTag->pf = m_mapMsgFunc[pMsgTag->fun_name];
				
			pMsgTag = pMsgTag->next;
		}
	}
}

void CBaseVision::USleep(unsigned int usec)
{
	struct timeval delay;
	
	delay.tv_sec = 0;
	delay.tv_usec = usec;
	
	select(0, NULL, NULL, NULL, &delay);
}

void CBaseVision::SetStatusCode(int code)
{
	if (code > 0)
	{
		RecoverErr(-code);
	}
	else
	{
		SetErr(code);
	}
}

void CBaseVision::NoticeTimer()
{
	pthread_cond_broadcast(&m_ready);
}

void CBaseVision::SendCanData(int identify, int id, char* pData, size_t size)
{
	if (NULL == pData)
	{
		return;
	}	

	CAN_SNT_DATA tSntData;
	
	tSntData.can_id = id;
	tSntData.data = pData;
	
	CHF::SetContent(identify, (char*)&tSntData, size);
}

unsigned CBaseVision::GetDelayTime(struct timeval* last_val, struct timeval* cur_val, unsigned dTime)
{
	unsigned int dt = (cur_val->tv_sec - last_val->tv_sec) * 1000000 + cur_val->tv_usec - last_val->tv_usec;
	
	return dt % dTime;
}

void CBaseVision::SetErr(int err)
{
	pthread_mutex_lock(&m_errLock);
	
	m_setErr.insert(err);
	
	set<int>::const_iterator its = m_setErr.end();
	if (its != m_setErr.begin())
	{
		--its;
		m_code = *its;
	}
	
	pthread_mutex_unlock(&m_errLock);
}

void CBaseVision::RecoverErr(int err)
{
	pthread_mutex_lock(&m_errLock);
	
	set<int>::const_iterator its = m_setErr.find(err);
	if (its != m_setErr.end())
	{
		m_setErr.erase(its);
	}
	
	pthread_mutex_unlock(&m_errLock);
}

