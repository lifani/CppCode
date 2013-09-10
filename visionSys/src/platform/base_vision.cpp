#include <platform/base_vision.h>
#include <mt/mt.h>
#include <sys/wait.h>
#include "monitor.h"

static void sig_chld(int signo)
{
	if (SIGCHLD == signo)
	{
		int status;
		pid_t pid = wait(&status);
		
		cout << "proc " << pid << " exit" << endl;
	}
}


// 发送心跳
static void SendHeartBeat(sigval_t st)
{
	CBaseVision* pBaseVision = (CBaseVision*)st.sival_ptr;
	
	HeartBeat beat;
	beat.type = pBaseVision->m_ppid;
	beat.pid = pBaseVision->m_pid;
	
	if (CMt::mt_send(pBaseVision->m_skey, (char*)&beat, sizeof(HeartBeat)) == -1)
	{
		pBaseVision->m_times--;
	}
	else
	{
		pBaseVision->m_times = 5;
	}
	
	if (0 == pBaseVision->m_times)
	{
		pBaseVision->Deactive();
	}
}

// 接收心跳
static void RecvHeartBeat(sigval_t st)
{
	CBaseVision* pBaseVision = (CBaseVision*)st.sival_ptr;
	
	vector<PROC_INFO>& vProcInfo = pBaseVision->m_vProcInfo;
	
	vector<PROC_INFO>::iterator itr = vProcInfo.begin();
	for(; itr != vProcInfo.end(); ++itr)
	{
		HeartBeat beat;
		beat.type = pBaseVision->m_pid;

		if (CMt::mt_recv(pBaseVision->m_rkey, (char*)&beat, sizeof(HeartBeat)) == -1)
		{
			itr->times = itr->times > 0 ? --itr->times : 0;
		}
		else
		{
			itr->times = 5;
		}
		
		if (itr->times == 0)
		{
			cout << "proc " << itr->pname << " lost connection" << endl;
		}
	}
}

void* StartPthread(void* arg)
{
	CBaseVision* p = (CBaseVision*)arg;
	
	p->Run();
	
	p->Deactive();
	
	return NULL;
}

// 创建定时器
int CreateTimer(FUNC func, timer_t *timerid, CBaseVision* pBaseVision)
{
	struct sigevent evt;
	memset(&evt, 0, sizeof(struct sigevent));
	
	evt.sigev_notify = SIGEV_THREAD;
	evt.sigev_notify_function = func;
	evt.sigev_value.sival_ptr = (void*)pBaseVision;

	return timer_create(CLOCK_REALTIME, &evt, timerid);
}

// 启动定时器
static int SetTimer(timer_t timerid)
{
	struct itimerspec new_value;
	
	new_value.it_interval.tv_sec = 5;
	new_value.it_interval.tv_nsec = 0;
	
	new_value.it_value.tv_sec = 5;  
    new_value.it_value.tv_nsec = 0; 
	
	return timer_settime(timerid, 0, &new_value, NULL);
}

// 删除定时器
static int DeleteTimer(timer_t timerid)
{
	return timer_delete(timerid);
}

// 响应函数

CBaseVision::CBaseVision(const char* ppname, const char* pname)
: m_ppname(ppname)
, m_pname(pname)
, m_ppid(0)
, m_pid(0)
, m_shmkey(0)
, m_skey(0)
, m_rkey(0)
, m_sTimerid(0)
, m_rTimerid(0)
, m_times(5)
, m_tid(0)
{
}

CBaseVision::~CBaseVision()
{
}

int CBaseVision::Active()
{
	GetProcInfo();
	
	signal(SIGCHLD, sig_chld);
	
	string strAbsName = "";
	
	// 存在父进程？
	if (!m_ppname.empty())
	{
		strAbsName = m_strCwd + string("/") + m_ppname;;
		// 注册消息发送模块  (ppname, ppid) : key
		if ((m_skey = CMt::mt_init(SEM_MODE, strAbsName.c_str(), m_ppid)) == -1)
		{
			return -1;
		}
		
		// 注册消息发送定时器 #key
		int nRet = CreateTimer(SendHeartBeat, &m_sTimerid, this);
		
		// 启动定时器
		nRet = SetTimer(m_sTimerid);
	}
	
	// 存在子进程？
	if (m_vProcInfo.size() > 0)
	{	
		strAbsName = m_strCwd + string("/") + m_pname;
		// 注册消息接收模块  (pname, pid)
		if ((m_rkey = CMt::mt_init(SEM_MODE, strAbsName.c_str(), m_pid)) == -1)
		{
			return -1;
		}
		
		// 注册消息接收定时器
		CreateTimer(RecvHeartBeat, &m_rTimerid, this);
		
		// 启动定时器
		SetTimer(m_rTimerid);
	}
	
	return 0;
}

int CBaseVision::Action()
{
	if (pthread_create(&m_tid, NULL, StartPthread, (void*)this) != 0)
	{
		return -1;
	}
	
	return 0;
}

int CBaseVision::Deactive()
{
	if (!m_ppname.empty())
	{
		// 注销通信模块
		CMt::mt_destory(m_shmkey);
		
		// 删除消息发送定时器
		DeleteTimer(m_sTimerid);
	}
	
	if (m_vProcInfo.size() > 0)
	{
		// 注销通信模块
		map<string, key_t>::iterator itr = m_mapShmKey.begin();
		for (; itr != m_mapShmKey.end(); ++itr)
		{
			CMt::mt_destory(itr->second);
		}
		
		// 注销消息模块
		CMt::mt_destory(m_rkey);
		
		// 删除消息接收定时器
		DeleteTimer(m_rTimerid);
	}

	NotifyExit(PROC_EXIT);
	
	return 0;
}

void CBaseVision::Run()
{
}

void CBaseVision::GetProcInfo()
{
	m_pid = getpid();
	m_ppid = getppid();
	
	char szPath[256] = {0};
	getcwd(szPath, sizeof(szPath));
	
	m_strCwd = string(szPath);
}

void CBaseVision::AddSonProcInfo(const char* pname, int pid)
{
	if (NULL == pname)
	{
		return;
	}
	
	PROC_INFO info;
	info.pname = pname;
	info.pid = pid;
	info.times = 5;
	
	m_vProcInfo.push_back(info);
}

int CBaseVision::SendData(key_t key, const char* ptr, int len)
{
	return CMt::mt_send(key, ptr, len);
}

int CBaseVision::RecvData(key_t key, char* ptr, int len)
{
	return CMt::mt_recv(key, ptr, len);
}
