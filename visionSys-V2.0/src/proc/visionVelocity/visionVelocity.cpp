/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "visionVelocity.h"
#include <VO/demon.h>

#define LOG_TAG "VISION_VELOCITY"

BEGAIN_MESSAGE_MAP(CVisionVelocity, CBaseVision)
	ON_COMMAND(VELOCITY_ID, &CVisionVelocity::ProcessMsg)
END_MESSAGE_MAP()

//BEGAIN_TIMER_MAP(CVisionVelocity, CBaseVision)
//	ON_TIMER(100000, &CVisionVelocity::StoreData)
//END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionVelocity)

CVisionVelocity::CVisionVelocity(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, m_Num(2)
, m_index(0)
, m_pImu(0)
, m_pFeedBack(0)
, m_pStereoBuf(0)
, m_pOutStereoBuf(0)
, m_pVoBuf(0)
, m_pOutVoBuf(0)
, m_pStereoInfoCtrl(0)
, m_pfStereoInfo(0)
, m_pfVoInfo(0)
{
}

CVisionVelocity::~CVisionVelocity()
{
}

/************************************
功能：	VisionVelocity激活函数，完成初始化工作
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionVelocity::ActiveImp()
{
	if (NULL == m_pFeedBack)
	{
		m_pFeedBack = new char[256];
		if (NULL == m_pFeedBack)
		{
			return -1;
		}
	}
	
	// 初始化选项
	InitOption();
	
	if (-1 == InitStore())
	{
		return -1;
	}
	
	// 注册算法接口
	Register(m_Num);
	
	LOGW("VisionVelocity actived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	实现各种资源的释放
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionVelocity::DeactiveImp()
{
	if (NULL != m_pFeedBack)
	{
		delete []m_pFeedBack;
		m_pFeedBack = NULL;
	}
	
	if (NULL != m_pStereoBuf)
	{
		delete []m_pStereoBuf;
		m_pStereoBuf = NULL;
	}
	
	if (NULL != m_pOutStereoBuf)
	{
		delete []m_pOutStereoBuf;
		m_pOutStereoBuf = NULL;
	}
	
	if (NULL != m_pVoBuf)
	{
		delete []m_pVoBuf;
		m_pVoBuf = NULL;
	}
	
	if (NULL != m_pOutVoBuf)
	{
		delete []m_pOutVoBuf;
		m_pOutVoBuf = NULL;
	}
	
	if (NULL != m_pStereoInfoCtrl)
	{
		delete m_pStereoInfoCtrl;
		m_pStereoInfoCtrl = NULL;
	}
	
	if (NULL != m_pVoInfoCtrl)
	{
		delete m_pVoInfoCtrl;
		m_pVoInfoCtrl = NULL;
	}
	
	if (NULL != m_pImuCtrl)
	{
		delete m_pImuCtrl;
		m_pImuCtrl = NULL;
	}
	
	if (NULL != m_pfStereoInfo)
	{
		fclose(m_pfStereoInfo);
		m_pfStereoInfo = NULL;
	}
	
	if (NULL != m_pfVoInfo)
	{
		fclose(m_pfVoInfo);
		m_pfVoInfo = NULL;
	}
	
	if (NULL != m_pfImu)
	{
		fclose(m_pfImu);
		m_pfImu = NULL;
	}
	
	LOGW("VisionVelocity deactived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	测速消息处理接口
参数：	无
返回：	无
************************************/
void CVisionVelocity::ProcessMsg(VISION_MSG* pMsg)
{
	if (NULL != pMsg)
	{
		m_pImu = (char*)(pMsg->data.ptr + 8 * MAX_CLOUD_SIZE);
		
		// 测速算法接口
		RunVelocity(pMsg->data.ptr, m_Num, m_pImu, (char*)m_pFeedBack, 
			(Stereo_Info*)m_pStereoBuf, (VO_Info*)m_pVoBuf);
		
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)m_pFeedBack;
		p->cnt = m_index++;
		
		pMsg->id = VELOCITY_BACK;
		pMsg->data.ptr = m_pFeedBack;
		pMsg->data.size = sizeof(CAN_VELOCITY_DATA);
		
		// 发送算法结果
		SendMsg(pMsg);
		
		//m_pStereoInfoCtrl->push(m_pStereoBuf);
		//m_pVoInfoCtrl->push(m_pVoBuf);
		//m_pImuCtrl->push(m_pImu);
	}
}

void CVisionVelocity::StoreData()
{
	char szBuf[512] = {0};
	
	if (-1 != m_pStereoInfoCtrl->pop(m_pOutStereoBuf))
	{
		for (int i = 0; i < m_Num; ++i)
		{
			Stereo_Info* pStereoInfo = (Stereo_Info*)(m_pOutStereoBuf + i * sizeof(Stereo_Info));
			//snprintf(szBuf, sizeof(szBuf) - 1 , "%d	%d	%d	%d	%d	%f	%f	%f	%f	%f\n"
			//	, pStereoInfo->LeftPointsNum, pStereoInfo->RightPointsNum
			//	, pStereoInfo->StereoMatchNum, pStereoInfo->FrameMatchNum
			//	, pStereoInfo->InlierNum, pStereoInfo->tbx
			//	, pStereoInfo->tby, pStereoInfo->tbz
			//	, pStereoInfo->weight, pStereoInfo->disparity);
			
			//snprintf(szBuf, sizeof(szBuf) - 1 , "%d	%d	%d	%d	%d	%f	%f	%f	%f	%f\n"
			//	, 10, 100, 1000, 10000, 1, 10.0, 100.0, 1000.0, 10000.0, 1.0);
				
			fwrite(szBuf, strlen(szBuf), 1, m_pfStereoInfo);
		}
	}
	
	if (-1 != m_pVoInfoCtrl->pop(m_pOutVoBuf))
	{
		VO_Info* pVoInfo = (VO_Info*)(m_pOutVoBuf);
		snprintf(szBuf, sizeof(szBuf) - 1, "%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\n"
			, pVoInfo->ctrl_pitch, pVoInfo->ctrl_roll, pVoInfo->ctrl_alti
			, pVoInfo->HoverFlag, (int)pVoInfo->KeyFrameFlag, (int)pVoInfo->isRotOK
			, (int)pVoInfo->isVelocityOK, (int)pVoInfo->isPositionOK
			, (int)pVoInfo->KeyFrameUpdateFlag, pVoInfo->Keyframe_Update_Cnt
			, pVoInfo->Keyframe_Failure_Cnt, pVoInfo->Quit_Sig_Cnt);
			
		fwrite(szBuf, strlen(szBuf), 1, m_pfVoInfo);
	}
	
	IMU_DATA imu;
	if (-1 != m_pImuCtrl->pop((char*)&imu))
	{
		snprintf(szBuf, sizeof(szBuf) - 1, "%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f\n",
		imu.acc_x, imu.acc_y, imu.acc_z,
		imu.gyro_x, imu.gyro_y, imu.gyro_z,
		imu.press,
		imu.q0, imu.q1, imu.q2, imu.q3,
		imu.vgx, imu.vgy, imu.vgz);
		
		fwrite(szBuf, strlen(szBuf), 1, m_pfImu);
	}
}

/************************************
功能：	初始化配置选项
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionVelocity::InitOption()
{
	vector<OPTION>::iterator itv = m_vOption.begin();
	for (; itv != m_vOption.end(); ++itv)
	{
		if ((itv->key).compare(string("num")) == 0)
		{
			m_Num = itv->value;
		}
	}
	
	return 0;
}

int CVisionVelocity::InitStore()
{
	m_pStereoBuf = new char[m_Num * sizeof(Stereo_Info)];
	if (NULL == m_pStereoBuf)
	{
		LOGE("malloc store buffer err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_pOutStereoBuf = new char[m_Num * sizeof(Stereo_Info)];
	if (NULL == m_pOutStereoBuf)
	{
		LOGE("malloc out store buffer err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_pVoBuf = new char[sizeof(VO_Info)];
	if (NULL == m_pVoBuf)
	{
		LOGE("malloc vo buffer err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_pOutVoBuf = new char[sizeof(VO_Info)];
	if (NULL == m_pOutVoBuf)
	{
		LOGE("malloc out vo buffer err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_pStereoInfoCtrl = new CQueueCtrl(m_Num * sizeof(Stereo_Info), 10, false);
	if (NULL == m_pStereoInfoCtrl)
	{
		LOGE("malloc stereo queue ctrl err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (-1 == m_pStereoInfoCtrl->Initialize())
	{
		LOGE("stereo queue ctrl init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_pVoInfoCtrl = new CQueueCtrl(sizeof(VO_Info), 10, false);
	if (NULL == m_pVoInfoCtrl)
	{
		LOGE("malloc stereo queue ctrl err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (-1 == m_pVoInfoCtrl->Initialize())
	{
		LOGE("vo info queue ctrl init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_pImuCtrl = new CQueueCtrl(sizeof(IMU_DATA), 10, false);
	if (NULL == m_pImuCtrl)
	{
		LOGE("malloc imu queue ctrl err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (-1 == m_pImuCtrl->Initialize())
	{
		LOGE("imu queue ctrl init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 备份
	if (-1 == MovePath("/data/v/stereo_info.txt"))
	{
		LOGE("back-up file %s err. %s : %d\n", "/data/v/stereo_info.txt", __FILE__, __LINE__);
	}
	
	m_pfStereoInfo = fopen("/data/v/stereo_info.txt", "w+");
	if (NULL == m_pfStereoInfo)
	{
		LOGE("open stereo info file err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 备份
	if (-1 == MovePath("/data/v/VO_info.txt"))
	{
		LOGE("back-up file %s err. %s : %d\n", "/data/v/VO_info.txt", __FILE__, __LINE__);
	}
	
	m_pfVoInfo = fopen("/data/v/VO_info.txt", "w+");
	if (NULL == m_pfVoInfo)
	{
		LOGE("open vo info file err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (-1 == MovePath("/data/v/imu.txt"))
	{
		LOGE("back-up file %s err. %s : %d\n", "/data/v/imu.txt", __FILE__, __LINE__);
	}
	
	m_pfImu = fopen("/data/v/imu.txt", "w+");
	if (NULL == m_pfImu)
	{
		LOGE("open imu file err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

int CVisionVelocity::ExecCmd(const char* strCmd)
{
	if (NULL == strCmd)
	{
		return -1;
	}
	
	FILE* pf = popen(strCmd, "r");
	if (NULL == pf)
	{
		return -1;
	}
	
	pclose(pf);
	
	return 0;
}

int CVisionVelocity::MovePath(const char* path)
{
	if (NULL == path)
	{
		return -1;
	}
	
	char szCmd[512] = {0};
	
	snprintf(szCmd, 512, "mv -f %s %s.bak", path, path);
	
	return ExecCmd(szCmd);
}


