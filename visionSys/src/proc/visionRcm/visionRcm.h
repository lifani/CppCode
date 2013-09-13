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

	int InitMMap();

	void process_poll(struct pollfd* p);
	
	bool isReady();
	
	void writeFlg(int fd);

	void Wait4FPGAReady();
	
	int WriteParameter(int type = 0);
	
	int ReadVelocityData(VELOCITY_DATA& tVelocity);
	
	int ReadRectifiedImg(RECTIFIED_IMG& tRectified);
	
private :

	bool m_bRunning;
	
	int st_fd;
	int m_fd;
	
	unsigned char* m_ptr;
	
	int m_sendFlg;
};

#endif
