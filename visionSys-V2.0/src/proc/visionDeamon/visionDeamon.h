/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.9
*************************************/
#ifndef __VISION_DEAMON_H__
#define __VISION_DEAMON_H__

#include <typedef.h>
#include <datatype.h>

#include <platform/base_vision.h>

class CVisionDeamon : public CBaseVision
{
	DECLARE_TIMER_MAP
	
public :

	CVisionDeamon(const char* ppname, const char* pname);
	
	virtual ~CVisionDeamon();
	
	virtual int ActiveImp();
	
	virtual int DeactiveImp();
};

#endif
