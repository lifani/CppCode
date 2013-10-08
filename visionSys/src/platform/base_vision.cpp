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
		
		LOGW("proc : %d exit. %s : %d\n", pid, __FILE__, __LINE__);
	}
}


// 发送心跳
void SendHeartBeat(sigval_t st)
{
	CBaseVision* pBaseVision = (CBaseVision*)st.sival_ptr;
	
	pBaseVision->SndHeartBeat();
}

// 接收心跳
void RecvHeartBeat(sigval_t st)
{
	CBaseVision* pBaseVision = (CBaseVision*)st.sival_ptr;
	
	pBaseVision->RcvHeartBeat();
}

void* StartPthread(void* arg)
{
	PTHREAD_PFUNC* pTFUNC = (PTHREAD_PFUNC*)arg;
	
	(pTFUNC->pBaseVision->*(pTFUNC->pFunc))();
	
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
, m_skey(0)
, m_rkey(0)
, m_sTimerid(0)
, m_rTimerid(0)
, m_times(5)
{
}

CBaseVision::~CBaseVision()
{
}

int CBaseVision::Active()
{
	GetProcInfo();
	
	signal(SIGCHLD, sig_chld);
	
	m_mapShmKey.clear();
	
	string strAbsName = "";

	// 存在父进程？
	if (!m_ppname.empty())
	{
		strAbsName = m_strCwd + string("/") + m_pname;
		
		key_t shmkey;
		// 注册通信模块
		if ((shmkey = CMt::mt_init(SHM_MODE, strAbsName.c_str(), m_pid, GetShmSize(m_pname), sizeof(FEEDBACK_DATA))) == -1)
		{
			LOGE("%s register share memory connection module error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
			return -1;
		}
		
		m_mapShmKey[m_pname] = shmkey;
	
		strAbsName = m_strCwd + string("/") + m_ppname;
		// 注册消息发送模块  (ppname, ppid) : key
		if ((m_skey = CMt::mt_init(SEM_MODE, strAbsName.c_str(), m_ppid)) == -1)
		{
			LOGE("%s register msg module error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
			return -1;
		}
		
		// 注册消息发送定时器 #key
		CreateTimer(SendHeartBeat, &m_sTimerid, this);
		
		// 启动定时器
		SetTimer(m_sTimerid);
	}
	
	// 存在子进程？
	if (m_vProcInfo.size() > 0)
	{
		// 注册通信模块
		vector<PROC_INFO>::iterator itr = m_vProcInfo.begin();
		for (; itr != m_vProcInfo.end(); ++itr)
		{
			strAbsName = m_strCwd + string("/") + itr->pname;

			key_t key = CMt::mt_init(SHM_MODE, strAbsName.c_str(), itr->pid, GetShmSize(itr->pname), sizeof(FEEDBACK_DATA));
			if (-1 == key)
			{
				LOGE("%s register share memory connection module error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
				return -1;
			}
			
			m_mapShmKey[itr->pname] = key;
		}
	
		strAbsName = m_strCwd + string("/") + m_pname;
		// 注册消息接收模块  (pname, pid)
		if ((m_rkey = CMt::mt_init(SEM_MODE, strAbsName.c_str(), m_pid)) == -1)
		{
			LOGE("%s register msg module error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
			return -1;
		}
		
		// 注册消息接收定时器
		CreateTimer(RecvHeartBeat, &m_rTimerid, this);
		
		// 启动定时器
		SetTimer(m_rTimerid);
	}
	
	// 默认注册线程函数Run
	RegisterPthread(&CBaseVision::Run);
	
	return 0;
}

int CBaseVision::Action()
{
	vector<PFUNC>::iterator itr = m_vPfunc.begin();
	for (; itr != m_vPfunc.end(); ++itr)
	{
		PTHREAD_PFUNC* tPfunc = new PTHREAD_PFUNC;
		
		tPfunc->pBaseVision = this;
		tPfunc->pFunc = *itr;

		if (pthread_create(&tPfunc->tid, NULL, StartPthread, (void*)tPfunc) != 0)
		{
			LOGE("create thread error. %s : %d\n", __FILE__, __LINE__);
			return -1;
		}
		
		m_vPthread.push_back(tPfunc);
	}
	
	return 0;
}

int CBaseVision::Deactive()
{
	if (!m_ppname.empty())
	{	
		// 删除消息发送定时器
		DeleteTimer(m_sTimerid);
	}
	
	if (m_vProcInfo.size() > 0)
	{	
		// 注销消息模块
		CMt::mt_destory(m_rkey);
		
		// 删除消息接收定时器
		DeleteTimer(m_rTimerid);
		
		char szCmd[256] = {0};
		vector<PROC_INFO>::iterator itrProc = m_vProcInfo.begin();
		for (; itrProc != m_vProcInfo.end(); ++itrProc)
		{	
			sprintf(szCmd, "kill -2 %d", itrProc->pid);
			
			FILE* fp = popen(szCmd, "r");
			if (NULL != fp)
			{
				pclose(fp);
			}
		}
	}
	
	// 注销通信模块
	map<string, key_t>::iterator itr = m_mapShmKey.begin();
	for (; itr != m_mapShmKey.end(); ++itr)
	{
		CMt::mt_destory(itr->second);
	}
	
	vector<PTHREAD_PFUNC*>::iterator it = m_vPthread.begin();
	for (; it != m_vPthread.end(); ++it)
	{
		// 停止线程
		delete *it;
		*it = NULL;
	}
	
	LOGW("%s deactived. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);

	NotifyExit(PROC_EXIT);
	
	return 0;
}

void CBaseVision::Run()
{

}

void CBaseVision::Run1()
{

}

void CBaseVision::Run2()
{

}

void CBaseVision::Run3()
{

}

void CBaseVision::SndHeartBeat()
{
	HeartBeat beat;
	beat.type = m_pid;
	
	unsigned int size = 0;
	if (CMt::mt_send(m_skey, (char*)&beat, &size) == -1)
	{
		m_times--;
	}
	else
	{
		m_times = 5;
	}
	
	if (0 == m_times)
	{
		LOGE("lost connection to server, deactive by itself. %s : %d\n", __FILE__, __LINE__);
		Deactive();
	}
}

void CBaseVision::RcvHeartBeat()
{
	vector<PROC_INFO>::iterator itr = m_vProcInfo.begin();
	for(; itr != m_vProcInfo.end();)
	{
		HeartBeat beat;
		beat.type = itr->pid;

		unsigned int size = 0;
		if (CMt::mt_recv(m_rkey, (char*)&beat, &size) == -1)
		{
			itr->times--;
		}
		else
		{
			itr->times = 5;
		}
		
		if (itr->times == 0)
		{
			LOGE("lost connection to %s. %s : %d\n", itr->pname.c_str(), __FILE__, __LINE__);
			itr = m_vProcInfo.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void CBaseVision::GetProcInfo()
{
	m_pid = getpid();
	m_ppid = getppid();
	
	char szPath[256] = {0};
	getcwd(szPath, sizeof(szPath));
	
	m_strCwd = string(szPath);
}

unsigned int CBaseVision::GetShmSize(string pname)
{
	unsigned int size = sizeof(SHM_DATA);
	if (pname.compare("visionVelocity") == 0)
	{
		size = sizeof(VELOCITY_DATA);
	}
	else if (pname.compare("visionBm") == 0)
	{
		size = sizeof(RECTIFIED_IMG);
	}
	else if (pname.compare("visionFightCtl") == 0)
	{
		size = sizeof(FIGHTCTL_DATA);
	}
	
	return size;
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

int CBaseVision::SendData(string pname, char* ptr, unsigned int* len)
{
	key_t key = m_mapShmKey[pname];
	
	return CMt::mt_send(key, ptr, len);
}

int CBaseVision::RecvData(string pname, char* ptr, unsigned int* len)
{
	key_t key = m_mapShmKey[pname];
	
	return CMt::mt_recv(key, ptr, len);
}

void CBaseVision::RegisterPthread(PFUNC func)
{
	m_vPfunc.push_back(func);
}

