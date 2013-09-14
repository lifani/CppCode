#ifndef __VISION_VELOCITY_H__
#define __VISION_VELOCITY_H__

#include <platform/base_vision.h>
#include <platform/visionStore.h>

class CVisionVelocity : public CBaseVision
{
public :

	CVisionVelocity(const char* ppname, const char* pname);
	
	virtual ~CVisionVelocity();
	
	virtual int Active();
	
	virtual int Deactive();
	
	virtual void Run();
	
	virtual void Run1();

private:
	
	bool m_bRunning;
	
	CVisionStore m_VisionStore;
};

#endif
