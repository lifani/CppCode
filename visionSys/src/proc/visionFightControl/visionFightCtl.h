#ifndef __VISION_FIGHT_CTL_H__
#define __VISION_FIGHT_CTL_H__

#include <platform/base_vision.h>

class CVisionFightCtl : public CBaseVision
{
public :
	
	CVisionFightCtl(const char* ppname, const char* pname);
	
	virtual ~CVisionFightCtl();
	
	virtual int Active();
	
	virtual int Deactive();
	
	virtual void Run();
	
private :

	bool m_bRunning;
};

#endif
