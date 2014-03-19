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
	
	//DECLARE_TIMER_MAP

public :

	CVisionVelocity(const char* ppname, const char* pname);
	
	virtual ~CVisionVelocity();

	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	void ProcessMsg(VISION_MSG* pMsg);
	
	void StoreData();

private :
	
	int InitOption();
	
	int InitStore();
	
	int ExecCmd(const char* strCmd);
	
	int MovePath(const char* path);

private :
	int m_Num;

	unsigned int m_index;

	char* m_pImu;
	char* m_pFeedBack;
	
	char* m_pStereoBuf;
	char* m_pOutStereoBuf;
	char* m_pVoBuf;
	char* m_pOutVoBuf;
	
	CQueueCtrl* m_pStereoInfoCtrl;
	CQueueCtrl* m_pVoInfoCtrl;
	CQueueCtrl* m_pImuCtrl;
	
	FILE* m_pfStereoInfo;
	FILE* m_pfVoInfo;
	FILE* m_pfImu;
};

#endif
