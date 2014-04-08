/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.4.4
*************************************/
#ifndef __VISION_BM_H__
#define __VISION_BM_H__

#include <typedef.h>

#include <platform/base_vision.h>

class CVisionBm : public CBaseVision
{
	DECLARE_MESSAGE_MAP
	
	DECLARE_TIMER_MAP

public :

	CVisionBm(const char* ppname, const char* pname);
	
	virtual ~CVisionBm();
	
	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	void ProcessMsg(VISION_MSG* pMsg);
	
};

#endif
