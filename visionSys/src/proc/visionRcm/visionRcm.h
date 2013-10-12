#ifndef __VISION_H__
#define __VISION_H__

#include <platform/base_vision.h>
#include <packet.h>

class CVisionRcm : public CBaseVision
{
public :
	
	CVisionRcm(const char* ppname, const char* pname);
	
	virtual ~CVisionRcm();
	
	virtual int Active();
	
	virtual int Deactive();

	virtual void Run();
	
	virtual void Run1();
	
	virtual void Run2();

private :

	int InitMMap();

	void process_poll(struct pollfd* p);
	
	bool isReady();
	
	void writeFlg(int fd);

	void Wait4FPGAReady(const char* file);
	
	int WriteParameter(int type = 0);
	
	int WriteCmos();
	
	int ReadVelocityData(VELOCITY_DATA& tVelocity);
	
	int ReadRectifiedImg(RECTIFIED_IMG& tRectified);
	
	void GetIMU(IMU& imu);
	
	void AddImu(Packet* p);
	
	void AddMc(Packet* p);
	
private :

	bool m_bRunning;
	bool m_isImuReady;
	bool m_isMcReady;
	
	int st_fd;
	int m_fd;
	
	unsigned char* m_ptr;
	
	can_interface* m_can0;
	imu_body m_imu_body[QUEUE_SIZE];
	MC	m_mc[QUEUE_SIZE];
	
	CAN_VELOCITY_DATA can_v[QUEUE_SIZE];
	CAN_BM_DATA can_b[QUEUE_SIZE];
	
	int m_FetchPos;
	int m_StorePos;
	
	int m_mcFetchPos;
	int m_mcStorePos;
	
	int m_can_v_fetch;
	int m_can_v_store;
	int m_can_b_fetch;
	int m_can_b_store;
	
	unsigned int m_index;
	
	unsigned char m_sndflg;
	
	vector<Filter_param> m_vFilter;
	
	static pthread_mutex_t imu_lock;
	static pthread_mutex_t can_lock;
	static pthread_cond_t  can_ready;
};

#endif
