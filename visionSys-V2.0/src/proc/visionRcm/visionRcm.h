/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __VISION_RCM_H__
#define __VISION_RCM_H__

#include <typedef.h>
#include <datatype.h>

#include <platform/base_vision.h>

#include "queueCtrl.h"
#include "../../common/can_data/CHF.h"

class CVisionRcm : public CBaseVision
{

	DECLARE_MESSAGE_MAP
	
	DECLARE_TIMER_MAP

public :

	CVisionRcm(const char* ppname, const char* pname);
	
	virtual ~CVisionRcm();
	
	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	void TransData();
	
	void ProcessVelocityMsg(VISION_MSG* pMsg);
	
	void SendCanData();
	
private :

	int Initialize();

	int InitMMap();
	
	int InitRegMMap();
	
	int InitOption();
	
	void process_poll(struct pollfd* p);
	
	bool isReady();
	
	void WriteFlg(int fd);
	
	void ReadData();
	
	void SendData();
	
	int Preprocess4Fpga();
	
	int WriteRbf();
	
	int Wait4FPGAReady();
	
	int WriteParameter();
	
	int WriteCmos();
	
	void EnableFPGA();
	
	void DisableFPGA();
	
	void EnableSonar();
	
	void DisableSonar();
	
	int GetImu(IMU_DATA* pImu);
	
private :

	int t_fd;
	int m_fd;
	int m_regFd;
	int m_can0;
	
	int m_Sonar;
	int m_Naza;
	int m_Num;
	int m_PegTop;
	int m_Folders;
	int m_Files;
	
	unsigned int m_sumSize;
	
	unsigned char* m_ptr;
	unsigned char* m_regPtr;
	unsigned char* m_pData;
	
	map<long, VISION_MSG*> m_mapMsg;
	
	CQueueCtrl m_qCtrl;
	
	IMU_DATA m_imu;
};

#endif
