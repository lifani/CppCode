/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.9
*************************************/
#ifndef __VISION_DAEMON_H__
#define __VISION_DAEMON_H__

#include <typedef.h>
#include <datatype.h>

#include <platform/base_vision.h>

class CVisionDaemon : public CBaseVision
{
	DECLARE_MESSAGE_MAP

	DECLARE_TIMER_MAP
	
public :

	CVisionDaemon(const char* ppname, const char* pname);
	
	virtual ~CVisionDaemon();
	
	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	virtual void Daemon();
	
	virtual void ProcessHeartMsg(VISION_MSG* pMsg);
	
private :

	int Initialize();

	int Restart(PROC_INFO* pProcInfo);
	
	void KillProc(pid_t pid);
	
	void SendAlarm();

private :
	
	int m_can;

	pthread_mutex_t m_lock;
};

#endif
