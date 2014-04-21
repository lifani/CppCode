/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "canPacket.h"

#include "newProtocolCanCtrl.h"
#include "sndCanCtrl.h"
#include "090CanCtrl.h"
#include "108CanCtrl.h"
#include "388CanCtrl.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

CCanPacket::CCanPacket() : m_fd(0), m_key(0), m_bitrate(1000000), m_nContent(0), m_index(0)
, m_rHdl((HANDLER)0)
, m_wHdl((HANDLER)0)
, m_388Queue(sizeof(MC), 10, 0)
, m_imuQueue(sizeof(IMU_DATA), 30, 20)
{
	pthread_mutex_init(&m_lock, NULL);
}

CCanPacket::~CCanPacket()
{	
	map<unsigned short, CAbstractCanCtrl*>::iterator itm = m_mapRdCanCtrl.begin();
	for (; itm != m_mapRdCanCtrl.end(); ++itm)
	{
		if (NULL != itm->second)
		{
			delete itm->second;
			itm->second = NULL;
		}
	}
	
	itm = m_mapWrCanCtrl.begin();
	for (; itm != m_mapWrCanCtrl.end(); ++itm)
	{
		if (NULL != itm->second)
		{
			delete itm->second;
			itm->second = NULL;
		}
	}
	
	pthread_mutex_destroy(&m_lock);
	
	LOGE("CCanPacket destroy.");
}

/************************************
功能：	获取CAN口描述符
参数：	identify const char* CAN口标识
		op int A2 0， NAZA 1
返回:	描述字 > 0, 失败 -1
************************************/
int CCanPacket::FD(const char* identify, int op)
{
	if (NULL == identify)
	{
		return -1;
	}
	
	Initialize(op);
	
	// create fd
	if (CreateFd(identify) != 0)
	{
		return -1;
	}
	
	return m_fd;
}

/************************************
功能：	读取CAN一帧数据
参数：	无
返回:	成功 0, 失败 -1
************************************/
int CCanPacket::ReadFd()
{
	struct can_frame frame;
	if (read(m_fd, &frame, sizeof(struct can_frame)) <= 0)
	{
		return -1;
	}

	Process(&frame);

	return 0;
}

/************************************
功能：	写一帧数据
参数：	无
返回:	成功 0, 失败 -1
************************************/
int CCanPacket::WriteFd()
{
	struct can_frame frame;
	
	map<unsigned short, CAbstractCanCtrl*>::iterator itm = m_mapWrCanCtrl.begin();
	for (; itm != m_mapWrCanCtrl.end(); ++itm)
	{	
		// 若失败，则再重试至成功为止
		if ( NULL != itm->second)
		{
			if (itm->second->Process(&frame) > 0)
			{
				while (write(m_fd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame))
				{
					usleep(2000);
					continue;
				}
				break;
			}
		}
	}
	
	return 0;
}

/************************************
功能：	获取一包IMU数据（包括0x090、0x388）
参数：	ptr char* 内容指针，不能为NULL
		len int*  内容长度，不能为NULL
返回:	无
************************************/
void CCanPacket::GetContent(char* ptr, int* len)
{
	if (NULL == ptr || NULL == len || 0 == *len)
	{
		return;
	}
	
	// 090数据
	IMU_DATA imu;
	m_imuQueue.pop((char*)&imu);
	
	// 388数据
	m_388Queue.pop((char*)&(imu.mc));

	// out
	*len = (unsigned int)*len > sizeof(IMU_DATA) ? sizeof(IMU_DATA) : *len;
	memcpy(ptr, (char*)&imu, *len);
}

/************************************
功能：	设置一包数据
参数：	ptr const char* 内容指针，不能为空
		len int	内容长度
返回:	无
************************************/
void CCanPacket::SetContent(const char* ptr, int len, int cmd)
{
	if (NULL == ptr || 0 == len)
	{
		return;
	}
	
	const CAN_SNT_DATA* pCanData = (CAN_SNT_DATA*)ptr;

	CAbstractCanCtrl* p = m_mapWrCanCtrl[pCanData->can_id];
	if (NULL != p)
	{
		if (-1 == p->SetContent(pCanData->data, len, cmd))
		{
			LOGE("push data err, can id = %d\n", pCanData->can_id);
		}
	}
}

/************************************
功能：	获取处理句柄
参数：	event short 事件
返回:	句柄
************************************/
HANDLER CCanPacket::GetHandler(short event)
{
	HANDLER hdl = NULL;
	
	if (event & POLLIN)
	{
		hdl = m_rHdl;
	}
	else if (event & POLLOUT)
	{
		hdl = m_wHdl;
	}
	
	return hdl;
}

/************************************
功能：	初始化
参数：	op int A2 0 , NAZA： 1
返回:	成功 0, 失败 -1
************************************/
int CCanPacket::Initialize(int op)
{	
	m_mapProcHandler[0x388] = &CCanPacket::Process388;
	m_mapProcHandler[0x109] = &CCanPacket::Process109;
	//m_mapProcHandler[0x108] = &CCanPacket::Process108;
	m_mapProcHandler[0x092] = &CCanPacket::Process092;
	//m_mapProcHandler[0x090] = &CCanPacket::Process090;
	
	//m_mapRdCanCtrl[0x090] = new C090CanCtrl;
	m_mapRdCanCtrl[0x092] = new CNewProtocolCanCtrl;
	//m_mapRdCanCtrl[0x108] = new C108CanCtrl;
	m_mapRdCanCtrl[0x109] = new CNewProtocolCanCtrl;
	m_mapRdCanCtrl[0x388] = new C388CanCtrl;
	
	m_mapWrCanCtrl[0x095] = new CSndCanCtrl;
	m_mapWrCanCtrl[0x608] = new CSndCanCtrl;
	m_mapWrCanCtrl[0x609] = new CSndCanCtrl;
	
	m_mapWrCanCtrl[0x095]->Initialize(0x095, 0x1005);
<<<<<<< HEAD
	m_mapWrCanCtrl[0x608]->Initialize(0x608, 0x100A);
=======
	m_mapWrCanCtrl[0x608]->Initialize(0x608, 0x100C);
>>>>>>> a840495036cc735b87d7e332d3869cf91ab10804
	m_mapWrCanCtrl[0x609]->Initialize(0x609, 0x100D);

	m_imuQueue.Initialize();
	m_388Queue.Initialize();
	
	m_rHdl = static_cast<HANDLER>(&CCanPacket::ReadFd);
	m_wHdl = static_cast<HANDLER>(&CCanPacket::WriteFd);
	
	return 0;
}

/************************************
功能：	获取CAN接口描述字
参数：	identify const char* CAN口标识
返回:	成功 0, 失败 -1
************************************/
int CCanPacket::CreateFd(const char* identify)
{
	int state = 0;
	if (can_get_state(identify, &state) < 0)
	{
		LOGE("get can state err, name : %s. %s : %d\n", identify, __FILE__, __LINE__);
		return -1;
	}
	
	if (0 != state)
	{
		if (ResetCan(identify) < 0)
		{
			return -1;
		}
	}
	else
	{
		struct can_bittiming can_bit;
		
		if (can_get_bittiming(identify, &can_bit) < 0)
		{
			LOGE("get bittiming err. %s : %d\n", __FILE__, __LINE__);
			return -1;
		}
		
		if (m_bitrate != can_bit.bitrate)
		{
			if (ResetCan(identify) < 0)
			{
				return -1;
			}
		}
	}

	if ((m_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		LOGE("get socket fail. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}

	struct ifreq ifr;
	strncpy(ifr.ifr_name, identify, sizeof(ifr.ifr_name));
	
	if (ioctl(m_fd, SIOCGIFINDEX, &ifr))
	{
		LOGE("ioctl fail. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	struct sockaddr_can addr;
	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
	{
		LOGE("bind socket fail. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 设置过滤条件
	struct can_filter rfilter[1];
	rfilter[0].can_id 	= 0x109;
	rfilter[0].can_mask = CAN_SFF_MASK;
	
	SetFilter(rfilter, sizeof(can_filter));
	
	return 0;
}

int CCanPacket::ResetCan(const char* identify)
{
	if (can_do_stop(identify) < 0)
	{
		LOGE("stop can error, name : %s. %s : %d\n", identify, __FILE__, __LINE__);
		return -1;
	}
	
	if (can_set_bitrate(identify, m_bitrate) < 0)
	{
		LOGE("set bitrate error, name : %s. %s : %d\n", identify, __FILE__, __LINE__);
		return -1;
	}
	
	if (can_do_start(identify) < 0)
	{
		LOGE("start can error, name : %s. %s : %d\n", identify, __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

/************************************
功能：	设置过滤条件
参数：	pFilter struct can_filter* 过滤条件
		size unsigned int 过滤条件数
返回:	无
************************************/
void CCanPacket::SetFilter(struct can_filter* pFilter, unsigned int size)
{
	if (NULL == pFilter)
	{
		return;
	}
	
	setsockopt(m_fd, SOL_CAN_RAW, CAN_RAW_FILTER, pFilter, size);
}

void CCanPacket::Process(struct can_frame* pFrame)
{
	if (NULL == pFrame)
	{
		return;
	}
	
	CAbstractCanCtrl* pCanCtrl = m_mapRdCanCtrl[pFrame->can_id];
	if (NULL == pCanCtrl)
	{
		return;
	}

	if (0 == pCanCtrl->Process(pFrame))
	{
		char* pData = NULL;
		int len = pCanCtrl->GetContent(pData);
		
		// 处理内容
		PROC_HANDLER hdl = m_mapProcHandler[pFrame->can_id];
	
		if (PROC_HANDLER(0) != hdl)
		{
			(this->*hdl)(pData, len);
		}
		
		usleep(1000);
	}
}

/************************************
功能：	处理388数据
参数：	ptr 内容 len  长度
返回:	无
************************************/
void CCanPacket::Process388(char* ptr, int len)
{
	if (NULL != ptr)
	{
		MC mc;
		
		if (0xA2 == *(ptr + 3))
		{
			ManageCtrlDataA2* p = (ManageCtrlDataA2*)ptr;
			
			// 赋值
			mc.pitch 	= p->g_real_input_control_core_pitch;
			mc.roll 	= p->g_real_input_control_core_roll;
			mc.alti 	= p->g_real_input_control_core_alti;	
			mc.aileron 	= p->g_real_input_channel_COMMAND_AILERON;
			mc.elevator = p->g_real_input_channel_COMMAND_ELEVATOR;
			mc.throttle = p->g_real_input_channel_COMMAND_THROTTLE;
			mc.rudder 	= p->g_real_input_channel_COMMAND_RUDDER;
			mc.coretail = p->g_real_input_control_core_tail;
		}
		else if (0xC2 == *(ptr + 3))
		{
			ManageCtrlDataC2* p = (ManageCtrlDataC2*)ptr;
			
			// 赋值
			mc.pitch 	= p->in_3;
			mc.roll 	= p->in_4;
			mc.alti 	= p->in_5;	
			mc.aileron 	= 0;
			mc.elevator = 0;
			mc.throttle = 0;
			mc.rudder 	= 0;
			mc.coretail = 0;
		}
		
		m_388Queue.push((char*)&mc);
	}
}

/************************************
功能：	处理109数据
参数：	ptr 内容 len  长度
返回:	无
************************************/
void CCanPacket::Process109(char* ptr, int len)
{
	if ((unsigned int)len >= sizeof(NormalMode) && NULL != ptr)
	{
		NormalMode* p = (NormalMode*)ptr;
	
		// 设置秘钥
		m_key = p->key;
		
		LOGW("key = %d. %s : %d\n", m_key, __FILE__, __LINE__);

		CAbstractCanCtrl* pCanCtrl = m_mapRdCanCtrl[0x092];
		if (NULL != pCanCtrl)
		{
			pCanCtrl->SetKey(m_key);
		}
		
		// 设置过滤条件
		struct can_filter rfilter[2];
		rfilter[0].can_id 	= 0x092;
		rfilter[0].can_mask = CAN_SFF_MASK;
		rfilter[1].can_id 	= 0x388;
		rfilter[1].can_mask = CAN_SFF_MASK;
		
		SetFilter(rfilter, sizeof(can_filter) * 2);
	}
}

/************************************
功能：	处理108数据
参数：	ptr 内容 len  长度
返回:	无
************************************/
void CCanPacket::Process108(char* ptr, int len)
{
	if ((unsigned int)len >= sizeof(NormalMode) && NULL != ptr)
	{
		NormalMode* p = (NormalMode*)ptr;
	
		// 设置秘钥
		m_key = p->key;
		
		LOGW("key = %d. %s : %d\n", m_key, __FILE__, __LINE__);

		CAbstractCanCtrl* pCanCtrl = m_mapRdCanCtrl[0x090];
		if (NULL != pCanCtrl)
		{
			pCanCtrl->SetKey(m_key);
		}
		
		// 设置过滤条件
		struct can_filter rfilter[2];
		rfilter[0].can_id 	= 0x090;
		rfilter[0].can_mask = CAN_SFF_MASK;
		rfilter[1].can_id 	= 0x388;
		rfilter[1].can_mask = CAN_SFF_MASK;
		
		SetFilter(rfilter, sizeof(can_filter) * 2);
	}
}

/************************************
功能：	处理092数据
参数：	ptr 内容 len  长度
返回:	无
************************************/
void CCanPacket::Process092(char* ptr, int len)
{
	if ((unsigned int)len >= sizeof(imu_body) && NULL != ptr)
	{
		imu_body* p = (imu_body*)ptr;
		
		IMU_DATA imu;
		
		//imu.acc_x = p->acc_x;
		imu.acc_x = m_index++;
		imu.acc_y = p->acc_y;
		imu.acc_z = p->acc_z;
				
		imu.gyro_x = p->gyro_x;
		imu.gyro_y = p->gyro_y;
		imu.gyro_z = p->gyro_z;
		
		imu.press = p->press;
		
		imu.q0 = p->q0;
		imu.q1 = p->q1;
		imu.q2 = p->q2;
		imu.q3 = p->q3;
		
		imu.vgx = p->vgx;
		imu.vgy = p->vgy;
		imu.vgz = p->vgz;

		m_imuQueue.push((char*)&imu);
	}
}

/************************************
功能：	处理090数据
参数：	ptr 内容 len  长度
返回:	无
************************************/
void CCanPacket::Process090(char* ptr, int len)
{
	if ((unsigned int)len >= sizeof(imu_body) && NULL != ptr)
	{
		imu_body* p = (imu_body*)ptr;
		
		IMU_DATA imu;
		
		imu.acc_x = p->acc_x;
		imu.acc_y = p->acc_y;
		imu.acc_z = p->acc_z;
				
		imu.gyro_x = p->gyro_x;
		imu.gyro_y = p->gyro_y;
		imu.gyro_z = p->gyro_z;
		
		imu.press = p->press;
		
		imu.q0 = p->q0;
		imu.q1 = p->q1;
		imu.q2 = p->q2;
		imu.q3 = p->q3;
		
		imu.vgx = p->vgx;
		imu.vgy = p->vgy;
		imu.vgz = p->vgz;

		m_imuQueue.push((char*)&imu);
	}
}


