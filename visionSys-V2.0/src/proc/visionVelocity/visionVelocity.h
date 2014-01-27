/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __VISION_VELOCITY_H__
#define __VISION_VELOCITY_H__

#include <platform/base_vision.h>

class CVisionVelocity : public CBaseVision
{	
	DECLARE_MESSAGE_MAP

public :

	CVisionVelocity(const char* ppname, const char* pname);
	
	virtual ~CVisionVelocity();

	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	void ProcessMsg(VISION_MSG* pMsg);

private :
	
	int InitOption();

private :
	int m_Num;

	unsigned int m_index;

	char* m_pImu;
	char* m_pFeedBack;
};

#endif
