/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "visionVelocity.h"
#include <VO/system_io.h>

#define LOG_TAG "VISION_VELOCITY"

BEGAIN_MESSAGE_MAP(CVisionVelocity, CBaseVision)
	ON_COMMAND(VELOCITY_ID, &CVisionVelocity::ProcessMsg)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionVelocity, CBaseVision)
	ON_TIMER(200000, false, &CBaseVision::SendHeartMsg)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionVelocity)

CVisionVelocity::CVisionVelocity(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, m_Num(2)
, m_index(0)
, m_pImu(0)
, m_pFeedBack(0)
, m_pfImu(NULL)
, m_queueCtrl(sizeof(IMU_DATA), 10, false)
{
	pthread_mutex_init(&m_lock, NULL);
	pthread_cond_init(&m_ready, NULL);
}

CVisionVelocity::~CVisionVelocity()
{
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_ready);
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
	
	m_pVoInfo = new char[sizeof(vo_info)];
	if (NULL == m_pVoInfo)
	{
		LOGE("malloc vo info buf err(%d).", errno);
		return -1;
	}
	
	m_pBranchInfo = new char[sizeof(branch_info) * m_Num];
	if (NULL == m_pBranchInfo)
	{
		LOGE("malloc branch info buf err(%d).", errno);
		return -1;
	}
	
	// 注册算法接口
	//if (!init_vo("/cache", m_Num))
	//{
	//	SetStatusCode(ERR_PROC_UNINTIED);
		
	//	LOGE("vo init err.");
	//	return -1;
	//}
	
	// 设置状态码
	SetStatusCode(ERR_INTIALIZED);
	
	m_queueCtrl.Initialize();
	
	m_pfImu = fopen("/data/v/imu.txt", "wb+");
	if (NULL == m_pfImu)
	{
		LOGE("open /data/v/imu.txt err.");
		return -1;
	}
	
	REGISTER_THREAD(&CVisionVelocity::StoreImu);
	
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
	if (NULL != pMsg && NULL != pMsg->data.ptr)
	{
		m_pImu = (char*)(pMsg->data.ptr + 2 * IMG_SIZE);
		
		IMU_DATA* pImu = (IMU_DATA*)m_pImu;
		
		//cout << pImu->press << " " << pImu->q0 << " " << pImu->q1 << " " << pImu->q2 << " " << pImu->q3 << endl;
		
		//pImu->press = 0.0;
		
		// 测速算法接口
		//run_vo(pMsg->data.ptr, m_pImu, 
		//	(vo_info*)m_pVoInfo, (branch_info*)m_pBranchInfo, (vo_can_output*)m_pFeedBack);
		
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)m_pFeedBack;
		p->cnt = m_index++;
		
		// 发送算法结果
		if (-1 == SendSmallMsg(VELOCITY_BACK, (char*)p, sizeof(CAN_VELOCITY_DATA)))
		{
			LOGE("send vo result msg err.");
		}
		
		pthread_mutex_lock(&m_lock);
		m_queueCtrl.push((char*)pImu);
		pthread_mutex_unlock(&m_lock);
		pthread_cond_signal(&m_ready);
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

void CVisionVelocity::StoreImu()
{
	while(IsRunning())
	{
		IMU_DATA imu;
	
		pthread_mutex_lock(&m_lock);
		while(-1 == m_queueCtrl.pop((char*)&imu))
		{
			pthread_cond_wait(&m_ready, &m_lock);
		}
		pthread_mutex_unlock(&m_lock);
		
		fprintf(m_pfImu, "%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f\n",
			imu.acc_x, imu.acc_y, imu.acc_z,
			imu.gyro_x, imu.gyro_y, imu.gyro_z,
			imu.press,
			imu.q0, imu.q1, imu.q2, imu.q3,
			imu.vgx, imu.vgy, imu.vgz);
		fflush(m_pfImu);
	}
}

