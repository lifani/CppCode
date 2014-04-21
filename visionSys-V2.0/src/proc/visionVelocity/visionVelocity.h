/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __VISION_VELOCITY_H__
#define __VISION_VELOCITY_H__

#include <typedef.h>

#include <platform/base_vision.h>
#include <platform/queueCtrl.h>

class CVisionVelocity : public CBaseVision
{	
	DECLARE_MESSAGE_MAP
	
	DECLARE_TIMER_MAP

public :

	CVisionVelocity(const char* ppname, const char* pname);
	
	virtual ~CVisionVelocity();

	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	void ProcessMsg(VISION_MSG* pMsg);
	
	void StoreImu();

private :
	
	int InitOption();
	
	int ExecCmd(const char* strCmd);
	
	int MovePath(const char* path);

private :
	int m_Num;

	unsigned int m_index;

	char* m_pImu;
	char* m_pFeedBack;
	
	char* m_pVoInfo;
	char* m_pBranchInfo;
	
	FILE* m_pfImu;
	FILE* m_pfTime;
	
	pthread_mutex_t m_lock;
	pthread_cond_t m_ready;
	
	CQueueCtrl m_queueCtrl;
	
	struct timeval start_time;
	struct timeval end_time;
};

#endif
