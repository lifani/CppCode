#ifndef __VISION_VELOCITY_H__
#define __VISION_VELOCITY_H__

#include <platform/base_vision.h>

class CVisionVelocity : public CBaseVision
{
public :

	CVisionVelocity(const char* ppname, const char* pname);
	
	virtual ~CVisionVelocity();
	
	virtual int Active();
	
	virtual int Deactive();
	
	virtual void Run();

private:
	
	bool m_bRunning;
};

#endif
