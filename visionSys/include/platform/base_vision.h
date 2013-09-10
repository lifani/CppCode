#ifndef __BASE_VISION_H__
#define __BASE_VISION_H__

#include <typedef.h>
#include <common_interface.h>

#include <signal.h>
#include <time.h>

typedef struct _PROC_INFO
{
	string pname;
	pid_t pid;
	int times;
} PROC_INFO;

typedef struct _HeartBeat
{
	long  type;
	pid_t pid;

} HeartBeat;

typedef struct _SHM_DATA
{
	char data[256];
} SHM_DATA;

typedef void (*FUNC) (union sigval val);

class CBaseVision;

// 发送心跳
static void SendHeartBeat(sigval_t st);

// 接收心跳
static void RecvHeartBeat(sigval_t st);

void* StartPthread(void* arg);


class CBaseVision : public CCommonInterface
{
	friend void SendHeartBeat(sigval_t st);
	
	friend void RecvHeartBeat(sigval_t st);
	
	friend void* StartPthread(void* arg);
	
public :
	
	CBaseVision(const char* ppname, const char* pname);
	
	virtual ~CBaseVision();
	
	virtual int Active();
	
	virtual int Action();
	
	virtual int Deactive();
	
	virtual void AddSonProcInfo(const char* pname, int pid);
	
	virtual void Run();
	
private :

	void GetProcInfo();
	
	int SendData(key_t key, const char* ptr, int len);
	
	int RecvData(key_t key, char* ptr, int len);
	
public :
	
	string m_ppname;
	string m_pname;
	string m_strCwd;
	
	pid_t m_ppid;
	pid_t m_pid;
	
	key_t m_shmkey;
	key_t m_skey;
	key_t m_rkey;

	timer_t m_sTimerid;
	timer_t m_rTimerid;
	
	int m_times;
	
	pthread_t m_tid;
	
	vector<PROC_INFO> m_vProcInfo;
	map<string, key_t> m_mapShmKey;
};

#endif
