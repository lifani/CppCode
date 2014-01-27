/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "canPacket.h"

#include "090CanCtrl.h"
#include "091CanCtrl.h"
#include "108CanCtrl.h"
#include "388CanCtrl.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

map<unsigned short, PROC_HANDLER> CCanPacket::g_mapProcHandler;

CCanPacket::CCanPacket() : m_fd(0), m_key(0), m_bitrate(1000000), m_nContent(0)
, m_388CanCtrl(NULL)
, m_108CanCtrl(NULL)
, m_090CanCtrl(NULL)
, m_rHdl((HANDLER)0)
, m_wHdl((HANDLER)0)
, m_388Queue(sizeof(MC), 10, true)
, m_090Queue(sizeof(IMU_DATA), 10, true)
{
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
	
	PROC_HANDLER hdl = m_mapProcHandler[frame.can_id];
	
	if (PROC_HANDLER(0) != hdl)
	{
		(this->*hdl)(&frame);
	}
	else
	{
		ChooseHandler(&frame);
	}

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
		while ( NULL != itm->second)
		{
			int err = itm->second->Process(&frame);
			if (err > 0)
			{
				if (write(m_fd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame))
				{
					usleep(2000);
					continue;
				}
			}
			else if (0 == err)
			{
				m_nContent = m_nContent > 0 ? (m_nContent - 1) : 0;
				if (0 == m_nContent)
				{
					m_wHdl = (HANDLER)0;
				}
			}
			
			break;
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
	m_090Queue.pop((char*)&imu);
	
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
void CCanPacket::SetContent(const char* ptr, int len)
{
	if (NULL == ptr || 0 == len)
	{
		return;
	}
	
	const CAN_SNT_DATA* pCanData = (CAN_SNT_DATA*)ptr;
	if (NULL == pCanData)
	{
		return;
	}
	
	CAbstractCanCtrl* p = m_mapWrCanCtrl[pCanData->can_id];
	if (NULL != p)
	{
		p->SetContent(pCanData->data, len);
		
		++m_nContent;
	}
	
	
	if (m_nContent > 0)
	{
		m_wHdl = static_cast<HANDLER>(&CCanPacket::WriteFd);
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
	m_uCanHead.szHead[0] = 0x55;
	m_uCanHead.szHead[1] = 0xaa;
	m_uCanHead.szHead[2] = 0x55;
	m_uCanHead.szHead[3] = 0xaa;
	
	m_mapProcHandler[0x388] = NULL;
	m_mapProcHandler[0x108] = NULL;
	m_mapProcHandler[0x090] = NULL;
	
	g_mapProcHandler[0x388] = &CCanPacket::Process388;
	g_mapProcHandler[0x108] = &CCanPacket::Process108;
	
	// first be setted NULL
	g_mapProcHandler[0x090] = NULL;
	
	if (op)
	{
		g_mapProcHandler[0x090] = &CCanPacket::Process090;
		g_mapProcHandler[0x108] = NULL;
	}
	
	m_mapRdCanCtrl[0x090] = new C090CanCtrl;
	m_mapRdCanCtrl[0x108] = new C108CanCtrl;
	m_mapRdCanCtrl[0x388] = new C388CanCtrl;
	
	m_mapWrCanCtrl[0x091] = new C091CanCtrl;
	
	m_mapWrCanCtrl[0x091]->Initialize(NULL);

	m_090Queue.Initialize();
	m_388Queue.Initialize();
	
	m_rHdl = static_cast<HANDLER>(&CCanPacket::ReadFd);
	m_wHdl = (HANDLER)0;
	
	return 0;
}

/************************************
功能：	获取CAN接口描述字
参数：	identify const char* CAN口标识
返回:	成功 0, 失败 -1
************************************/
int CCanPacket::CreateFd(const char* identify)
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
	rfilter[0].can_id 	= 0x108;
	rfilter[0].can_mask = CAN_SFF_MASK;
	
	SetFilter(rfilter, sizeof(can_filter));
	
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

/************************************
功能：	处理388数据帧
参数：	pFrame struct can_frame* 数据帧
返回:	无
************************************/
void CCanPacket::Process388(struct can_frame* pFrame)
{
	m_388CanCtrl = m_mapRdCanCtrl[pFrame->can_id];
	if (NULL == pFrame || NULL == m_388CanCtrl)
	{
		return;
	}
	
	int err = m_388CanCtrl->Process(pFrame);
	if ( err > 0)
	{
		char* pData = NULL;
		
		int len = m_388CanCtrl->GetContent(pData);
		if ((unsigned int)len >= sizeof(ManageControlData) && NULL != pData)
		{
			ManageControlData* p = (ManageControlData*)pData;
		
			MC mc;
			
			// 赋值
			mc.pitch 	= p->g_real_input_control_core_pitch;
			mc.roll 	= p->g_real_input_control_core_roll;
			mc.alti 	= p->g_real_input_control_core_alti;	
			mc.aileron 	= p->g_real_input_channel_COMMAND_AILERON;
			mc.elevator = p->g_real_input_channel_COMMAND_ELEVATOR;
			mc.throttle = p->g_real_input_channel_COMMAND_THROTTLE;
			mc.rudder 	= p->g_real_input_channel_COMMAND_RUDDER;
			mc.coretail = p->g_real_input_control_core_tail;
			
			m_388Queue.push((char*)&mc);
			
			m_mapProcHandler[0x388] = NULL;
		}
		
		usleep(3000);
	}
	else if (-1 == err)
	{
		m_mapProcHandler[0x388] = NULL;
		
		usleep(3000);
	}
}

/************************************
功能：	处理108数据帧
参数：	pFrame struct can_frame* 数据帧
返回:	无
************************************/
void CCanPacket::Process108(struct can_frame* pFrame)
{
	m_108CanCtrl = m_mapRdCanCtrl[pFrame->can_id];
	if (NULL == pFrame || NULL == m_108CanCtrl)
	{
		return;
	}
	
	int err = m_108CanCtrl->Process(pFrame);
	if (err > 0)
	{
		char* pData = NULL;
		int len = m_108CanCtrl->GetContent(pData);
		if ((unsigned int)len >= sizeof(NormalMode) && NULL != pData)
		{
			NormalMode* p = (NormalMode*)pData;
		
			// 设置秘钥
			m_key = p->key;
			
			LOGW("key = %d. %s : %d\n", m_key, __FILE__, __LINE__);

			m_090CanCtrl = m_mapRdCanCtrl[0x090];
			if (NULL != m_090CanCtrl)
			{
				m_090CanCtrl->SetKey(m_key);
			}
			
			// 设置过滤条件
			struct can_filter rfilter[2];
			rfilter[0].can_id 	= 0x090;
			rfilter[0].can_mask = CAN_SFF_MASK;
			rfilter[1].can_id 	= 0x388;
			rfilter[1].can_mask = CAN_SFF_MASK;
			
			SetFilter(rfilter, sizeof(can_filter) * 2);
			
			// 切换工作模式
			m_mapProcHandler[0x108] = NULL;
			g_mapProcHandler[0x108] = NULL;
			g_mapProcHandler[0x090] = &CCanPacket::Process090;
		}
	}
	else if (-1 == err)
	{
		m_mapProcHandler[0x108] = NULL;
	}
}

/************************************
功能：	处理090数据帧
参数：	pFrame struct can_frame* 数据帧
返回:	无
************************************/
void CCanPacket::Process090(struct can_frame* pFrame)
{
	m_090CanCtrl = m_mapRdCanCtrl[pFrame->can_id];
	if (NULL == pFrame || NULL == m_090CanCtrl)
	{
		return;
	}
	
	int err = m_090CanCtrl->Process(pFrame);
	if (err > 0)
	{
		char* pData = NULL;
		
		int len = m_090CanCtrl->GetContent(pData);
		if ((unsigned int)len >= sizeof(imu_body) && NULL != pData)
		{
			imu_body* p = (imu_body*)pData;
			
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

			m_090Queue.push((char*)&imu);

			m_mapProcHandler[0x090] = NULL;
		}
		
		usleep(3000);
	}
	else if (-1 == err)
	{
		m_mapProcHandler[0x090] = NULL;
		
		usleep(3000);
	}
}

/************************************
功能：	选择处理句柄
参数：	pFrame struct can_frame* 数据帧
返回:	无
************************************/
void CCanPacket::ChooseHandler(struct can_frame* pFrame)
{	
	if (NULL == pFrame || FRAME_LEN != pFrame->can_dlc)
	{
		return;
	}

	unsigned int uHead = *((unsigned int*)pFrame->data);
	
	if (m_uCanHead.uHead == uHead)
	{
		CAbstractCanCtrl* pCan = m_mapRdCanCtrl[pFrame->can_id];
		if (NULL == pCan)
		{
			return;
		}

		if (-1 == pCan->Initialize(pFrame))
		{
			return;
		}
	
		PROC_HANDLER hdl = g_mapProcHandler[pFrame->can_id];
		if (PROC_HANDLER(0) != hdl)
		{
			m_mapProcHandler[pFrame->can_id] = hdl;
		}
	}
}


