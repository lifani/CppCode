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


// ��������
void SendHeartBeat(sigval_t st)
{
	CBaseVision* pBaseVision = (CBaseVision*)st.sival_ptr;
	
	HeartBeat beat;
	beat.type = pBaseVision->m_ppid;
	beat.pid = pBaseVision->m_pid;
	
	unsigned int size = sizeof(HeartBeat);
	if (CMt::mt_send(pBaseVision->m_skey, (char*)&beat, &size) == -1)
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

// ��������
void RecvHeartBeat(sigval_t st)
{
	CBaseVision* pBaseVision = (CBaseVision*)st.sival_ptr;
	
	vector<PROC_INFO>& vProcInfo = pBaseVision->m_vProcInfo;
	
	vector<PROC_INFO>::iterator itr = vProcInfo.begin();
	for(; itr != vProcInfo.end(); ++itr)
	{
		HeartBeat beat;
		beat.type = pBaseVision->m_pid;

		unsigned int size = sizeof(HeartBeat);
		if (CMt::mt_recv(pBaseVision->m_rkey, (char*)&beat, &size) == -1)
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
	PTHREAD_PFUNC* pTFUNC = (PTHREAD_PFUNC*)arg;
	
	(pTFUNC->pBaseVision->*(pTFUNC->pFunc))();
	
	pTFUNC->pBaseVision->Deactive();
	
	return NULL;
}

// ������ʱ��
int CreateTimer(FUNC func, timer_t *timerid, CBaseVision* pBaseVision)
{
	struct sigevent evt;
	memset(&evt, 0, sizeof(struct sigevent));
	
	evt.sigev_notify = SIGEV_THREAD;
	evt.sigev_notify_function = func;
	evt.sigev_value.sival_ptr = (void*)pBaseVision;

	return timer_create(CLOCK_REALTIME, &evt, timerid);
}

// ������ʱ��
static int SetTimer(timer_t timerid)
{
	struct itimerspec new_value;
	
	new_value.it_interval.tv_sec = 5;
	new_value.it_interval.tv_nsec = 0;
	
	new_value.it_value.tv_sec = 5;  
    new_value.it_value.tv_nsec = 0; 
	
	return timer_settime(timerid, 0, &new_value, NULL);
}

// ɾ����ʱ��
static int DeleteTimer(timer_t timerid)
{
	return timer_delete(timerid);
}

// ��Ӧ����

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

	// ���ڸ����̣�
	if (!m_ppname.empty())
	{
		strAbsName = m_strCwd + string("/") + m_pname;
		

		key_t shmkey;
		// ע��ͨ��ģ��
		if ((shmkey = CMt::mt_init(SHM_MODE, strAbsName.c_str(), m_pid, GetShmSize(m_pname), sizeof(FEEDBACK_DATA))) == -1)
		{
			return -1;
		}
		
		m_mapShmKey[m_pname] = shmkey;
	
		strAbsName = m_strCwd + string("/") + m_ppname;
		// ע����Ϣ����ģ��  (ppname, ppid) : key
		if ((m_skey = CMt::mt_init(SEM_MODE, strAbsName.c_str(), m_ppid)) == -1)
		{
			return -1;
		}
		
		// ע����Ϣ���Ͷ�ʱ�� #key
		int nRet = CreateTimer(SendHeartBeat, &m_sTimerid, this);
		
		// ������ʱ��
		nRet = SetTimer(m_sTimerid);
	}
	
	// �����ӽ��̣�
	if (m_vProcInfo.size() > 0)
	{
		// ע��ͨ��ģ��
		vector<PROC_INFO>::iterator itr = m_vProcInfo.begin();
		for (; itr != m_vProcInfo.end(); ++itr)
		{
			strAbsName = m_strCwd + string("/") + itr->pname;

			key_t key = CMt::mt_init(SHM_MODE, strAbsName.c_str(), itr->pid, GetShmSize(itr->pname), sizeof(FEEDBACK_DATA));
			if (-1 == key)
			{
				return -1;
			}
			
			m_mapShmKey[itr->pname] = key;
		}
	
		strAbsName = m_strCwd + string("/") + m_pname;
		// ע����Ϣ����ģ��  (pname, pid)
		if ((m_rkey = CMt::mt_init(SEM_MODE, strAbsName.c_str(), m_pid)) == -1)
		{
			return -1;
		}
		
		// ע����Ϣ���ն�ʱ��
		CreateTimer(RecvHeartBeat, &m_rTimerid, this);
		
		// ������ʱ��
		SetTimer(m_rTimerid);
	}
	
	// Ĭ��ע���̺߳���Run
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
		// ɾ����Ϣ���Ͷ�ʱ��
		DeleteTimer(m_sTimerid);
	}
	
	if (m_vProcInfo.size() > 0)
	{	
		// ע����Ϣģ��
		CMt::mt_destory(m_rkey);
		
		// ɾ����Ϣ���ն�ʱ��
		DeleteTimer(m_rTimerid);
		
		vector<PROC_INFO>::iterator itrProc = m_vProcInfo.begin();
		for (; itrProc != m_vProcInfo.end(); ++itrProc)
		{	
			kill(itrProc->pid, SIGINT);
		}
	}
	
	// ע��ͨ��ģ��
	map<string, key_t>::iterator itr = m_mapShmKey.begin();
	for (; itr != m_mapShmKey.end(); ++itr)
	{
		CMt::mt_destory(itr->second);
	}
	
	vector<PTHREAD_PFUNC*>::iterator it = m_vPthread.begin();
	for (; it != m_vPthread.end(); ++it)
	{
		// ֹͣ�߳�
		delete *it;
		*it = NULL;
	}

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

