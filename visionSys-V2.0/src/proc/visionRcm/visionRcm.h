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
#include <platform/queueCtrl.h>

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
	
	void SendVoData();
	
	void SendCanData(int identify, int id, char* pData, size_t size);
	
	int GetImu(VISION_MSG* pMsg, int beginPos, int offset);
	
	int GetDataFromFpga(VISION_MSG* pMsg, int beginPos, int offset);
	
	int GetVCtrl(VISION_MSG* pMsg, int beginPos, int offset);
	
	int Prepare4Vo(VISION_MSG* pMsg, int beginPos, int offset);
	
private :

	int Initialize();

	int InitMMap();
	
	int InitRegMMap();
	
	int InitOption();
	
	int process_poll(struct pollfd* p);
	
	bool isReady();
	
	void WriteFlg(int fd);
	
	void GenerateMsg();
	
	int Preprocess4Fpga();
	
	int WriteRbf();
	
	int Wait4FPGAReady();
	
	int WriteParameter();
	
	int WriteCmos();
	
	void EnableFPGA();
	
	void DisableFPGA();
	
	void EnableSonar();
	
	void DisableSonar();
	
	void GetImuFromCan();

#ifdef _RUN_SMALL_ALG_

	void RunAlgTask();

#endif
	
	void Prepare4Bm(RECTIFIED_IMG* pRecInfo);
	
	void OutTime(FILE* pf);
	
private :

	int t_fd;
	int m_fd;
	int m_regFd;
	int m_can0;
	int m_subs;
	
	int m_Sonar;
	int m_Naza;
	int m_Num;
	int m_PegTop;
	int m_Folders;
	int m_Files;
	int m_offset_d;
	int m_offset_l;
	int m_offset_r;
	int m_discards;
	
	unsigned int m_index4Vo;
	
	unsigned int m_sumSize;
	
	unsigned char* m_ptr;
	unsigned char* m_regPtr;
	unsigned char* m_pData;
	
	CQueueCtrl m_qCtrl;
	
	IMU_DATA m_imu;
	
	bool m_work;
	
	unsigned m_index;
	
	struct timeval start_time;
    struct timeval end_time;
	
	FILE* m_pf0;
	FILE* m_pf1;
	
	bool m_bFirst;
};

#endif
