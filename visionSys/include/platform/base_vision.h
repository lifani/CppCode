#ifndef __BASE_VISION_H__
#define __BASE_VISION_H__

#include <typedef.h>
#include <common_interface.h>

#include <signal.h>
#include <time.h>

#include <datatype.h>


// 发送心跳
void SendHeartBeat(sigval_t st);

// 接收心跳
void RecvHeartBeat(sigval_t st);

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
	
	// 预备线程函数
	virtual void Run1();
	
	virtual void Run2();
	
	virtual void Run3();
	
	int SendData(string pname, char* ptr, unsigned int* len);
	
	int RecvData(string pname, char* ptr, unsigned int* len);
	
	void RegisterPthread(PFUNC func);
	
private :

	void GetProcInfo();
	
	unsigned int GetShmSize(string pname);
	
public :
	
	string m_ppname;
	string m_pname;
	string m_strCwd;
	
	pid_t m_ppid;
	pid_t m_pid;

	key_t m_skey;
	key_t m_rkey;

	timer_t m_sTimerid;
	timer_t m_rTimerid;
	
	int m_times;
	
	vector<PROC_INFO> m_vProcInfo;
	map<string, key_t> m_mapShmKey;
	
	vector<PFUNC> m_vPfunc;
	vector<PTHREAD_PFUNC*> m_vPthread;
};

#endif
