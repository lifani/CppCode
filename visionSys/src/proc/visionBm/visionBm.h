#ifndef __VISION_BM_H__
#define __VISION_BM_H__

#include <platform/base_vision.h>
#include <platform/visionStore.h>

class CVisionBm : public CBaseVision
{
public :
	
	CVisionBm(const char* ppname, const char* pname);
	
	virtual ~CVisionBm();
	
	virtual int Active();
	
	virtual int Deactive();
	
	virtual void Run();
	
	virtual void Run1();

private :

	bool m_bRunning;
	
	CVisionStore m_VisionStore;
};

#endif
