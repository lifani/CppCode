#ifndef __VISION_H__
#define __VISION_H__

#include <platform/base_vision.h>

class CVisionRcm : public CBaseVision
{
public :
	
	CVisionRcm(const char* ppname, const char* pname);
	
	virtual ~CVisionRcm();
	
	virtual int Active();
	
	virtual int Deactive();

	virtual void Run();
	
	virtual void Run1();

private :

	bool m_bRunning;
};

#endif
