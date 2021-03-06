/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "visionRcm.h"

#ifdef _RUN_SMALL_ALG_
#include <bm/runbm.h>
#endif

#undef LOG_TAG
#define LOG_TAG "VISION_RCM"

BEGAIN_MESSAGE_MAP(CVisionRcm, CBaseVision)
	ON_COMMAND(VELOCITY_BACK, &CVisionRcm::ProcessVelocityMsg)
	ON_COMMAND(RCM_LOG, &CVisionRcm::OutLog)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionRcm, CBaseVision)
	ON_TIMER(200000, false, &CBaseVision::SendHeartMsg)
	ON_TIMER(100000, true, &CVisionRcm::SendVoData)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionRcm)

CVisionRcm::CVisionRcm(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, t_fd(0)
, m_fd(0)
, m_regFd(0)
, m_can0(0)
, m_can1(0)
, m_subs(0)
, m_Sonar(0)
, m_Naza(0)
, m_Num(0)
, m_PegTop(0)
, m_Folders(0)
, m_Files(0)
, m_offset_d(0)
, m_offset_l(0)
, m_offset_r(0)
, m_discards(0)
, m_index4Vo(0)
, m_sumSize(0)
, m_ptr(0)
, m_regPtr(0)
, m_pData(0)
, m_qCtrl(sizeof(CAN_VELOCITY_DATA), 10, false)
, m_work(false)
, m_bImuErr(false)
, m_index(0)
, m_bFirst(true)
{
}

CVisionRcm::~CVisionRcm()
{
}

/************************************
功能：	VisionRcm激活函数，完成初始化工作
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::ActiveImp()
{
	if (0 != Initialize())
	{
		LOGE("Initialize fail. %s : %d\n", __FILE__, __LINE__);
		
		return -1;
	}

	// 注册线程函数
	REGISTER_THREAD(&CVisionRcm::TransData);
	
	// 注册消息生成函数
	REGISTER_MSG_FUNC("GetDataFromFpga", &CVisionRcm::GetDataFromFpga);
	REGISTER_MSG_FUNC("GetImu", &CVisionRcm::GetImu);
	REGISTER_MSG_FUNC("VO", &CVisionRcm::Prepare4Vo);
	
	LOGW("VisionRcm actived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	实现各种资源的释放
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::DeactiveImp()
{
	DisableFPGA();
	
	CHF::Destroy();
	
	// 关闭轮询文件描述符
	close(t_fd);
	
	// 注销文件映射存储
	if (NULL != m_ptr)
	{
		munmap(m_ptr, MMAP_SIZE);
		close(m_fd);
	}
	
	if (NULL != m_regPtr)
	{
		munmap(m_regPtr, 1024);
		close(m_regFd);
	}
	
	if (NULL != m_pData)
	{
		delete []m_pData;
		m_pData = NULL;
	}
	
#ifdef _RUN_SMALL_ALG_
	ReleseBm();
#endif
	
	LOGW("VisionRcm deactived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	主线程，由数据驱动。完成数据读取、传递。
参数：	无
返回：	无
************************************/
void CVisionRcm::TransData()
{
	struct pollfd p_fd;

	while(IsRunning())
	{
		p_fd.fd = t_fd;
		p_fd.events = POLLERR | POLLHUP;
	
		// 调用轮询函数
		if (0 != process_poll(&p_fd))
		{
			// 将轮询标志位置0
			WriteFlg(t_fd);
			continue;
		}

		if (m_bFirst)
		{
			// 通知Timer结束等待
			NoticeTimer();
			m_bFirst = false;
		}
		
		// 生成消息并发送
		GenerateMsg();
		
#ifdef _RUN_SMALL_ALG_
		// 算法任务
		RunAlgTask();
#endif
		// 将轮询标志位置0
		WriteFlg(t_fd);

	}
}

/************************************
功能：	测速结果消息处理接口
参数：	无
返回：	无
************************************/
void CVisionRcm::ProcessVelocityMsg(VISION_MSG* pMsg)
{	
	if (NULL != pMsg)
	{
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)pMsg->data.y.buf;

		m_qCtrl.push((char*)p);
	}
}

/************************************
功能：	通过CAN口发送数据
参数：	无
返回：	无 
************************************/
void CVisionRcm::SendVoData()
{	
	CAN_VELOCITY_DATA data;
	
	if (m_qCtrl.pop((char*)&data) == 0)
	{
<<<<<<< HEAD
=======
		OutTime(m_pf1);
>>>>>>> a840495036cc735b87d7e332d3869cf91ab10804
		SendCanData(m_can0, 0x095, (char*)&data, sizeof(CAN_VELOCITY_DATA));
	}
}

int CVisionRcm::GetImu(VISION_MSG* pMsg, int beginPos, int offset)
{
	if (NULL != pMsg->data.ptr)
	{
		IMU_DATA* pImu = (IMU_DATA*)(pMsg->data.ptr + pMsg->data.x.size);
		
		*pImu = m_imu;
		
		pMsg->data.x.size += sizeof(IMU_DATA);
	}
	
	return 0;
}

int CVisionRcm::GetDataFromFpga(VISION_MSG* pMsg, int beginPos, int offset)
{
	if (NULL != pMsg->data.ptr)
	{
		memcpy(pMsg->data.ptr + pMsg->data.x.size, m_ptr + beginPos, offset);
		pMsg->data.x.size += offset;
	}
	
	return 0;
}

int CVisionRcm::GetVCtrl(VISION_MSG* pMsg, int beginPos, int offset)
{
	return 0;
}

int CVisionRcm::Prepare4Vo(VISION_MSG* pMsg, int beginPos, int offset)
{
	if (m_index4Vo++ % 2 != 0)
	{
		return -1;
	}
	
	if (NULL != pMsg->data.ptr)
	{
		GetDataFromFpga(pMsg, beginPos, offset);
		GetImu(pMsg, beginPos, offset);
	}
	
	return 0;
}

/************************************
功能：	实现各种资源的初始化
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::Initialize()
{
	m_pData = new unsigned char[MSG_MEM_SIZE / 2];
	if (NULL == m_pData)
	{
		LOGE("malloc memory err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}

	// 初始化选项
	InitOption();
	
	// 初始化队列管理器
	if (0 != m_qCtrl.Initialize())
	{
		LOGE("queue contrl module init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 得到描述字
	m_can0 = CHF::FD(HF_CAN0, m_Naza);
	if (-1 == m_can0)
	{
		LOGE("Get can0 interface err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_can1 = CHF::FD(HF_CAN1, m_Naza);
	if (-1 == m_can1)
	{
		LOGE("Get can1 interface err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	/*m_subs = CHF::FD(HF_COM);
	if (-1 == m_subs)
	{
		LOGE("get com interface err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}*/
	
	// 完成CHF的初始化工作
	if (-1 == CHF::Initialize())
	{
		LOGE("CHF init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("CHF init succeed. %s : %d\n", __FILE__, __LINE__);
	
	// 轮询描述符
	if ((t_fd = open(DEVICE_SYS_POLL, O_RDWR)) < 0)
	{
		LOGE("open poll fd error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 文件映射存储
	if (-1 == InitMMap())
	{
		LOGE("mmap error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("map fpga memory succeed. %s : %d\n", __FILE__, __LINE__);
	
	// 映射寄存器
	if (-1 == InitRegMMap())
	{
		LOGE("map register err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("map register succeed. %s : %d\n", __FILE__, __LINE__);
	
	// FPGA预处理
	if (-1 == Preprocess4Fpga())
	{
		LOGE("Prepare for fpga err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("Prepare for fpga succeed. %s : %d\n", __FILE__, __LINE__);
	
	// 使能超声波
	if (m_Sonar)
	{
		EnableSonar();
	}
	
#ifdef _RUN_SMALL_ALG_

	RegisterBm();

#endif
	
	return 0;
}

/************************************
功能：	文件映射存储
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::InitMMap()
{
	m_fd = open(DEVICE_FILENAME, O_RDWR);
	if (m_fd < 0)
	{
		return -1;
	}
	
	m_ptr = (unsigned char*)mmap(0, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	if (NULL == m_ptr)
	{
		close(m_fd);
		m_fd = 0;
		
		return -1;
	}
	
	return 0;
}

/************************************
功能：	映射寄存器地址
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::InitRegMMap()
{
	m_regFd = open("/dev/mem", O_RDWR | O_SYNC);
	if (-1 == m_regFd)
	{
		LOGE("open /dev/mem err. %s : %d", __FILE__, __LINE__);
		return -1;
	}
	
	m_regPtr = (unsigned char*)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED,
		m_regFd, FPGA_HREG);
	if (NULL == m_regPtr)
	{
		LOGE("map register err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

/************************************
功能：	初始化配置选项
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::InitOption()
{
	vector<OPTION>::iterator itv = m_vOption.begin();
	for (; itv != m_vOption.end(); ++itv)
	{
		if ((itv->key).compare(string("naza")) == 0)
		{
			m_Naza = itv->value;
		}
		
		if ((itv->key).compare(string("num")) == 0)
		{
			m_Num = itv->value;
		}
		
		if ((itv->key).compare(string("sonar")) == 0)
		{
			m_Sonar = itv->value;
		}
		
		if ((itv->key).compare(string("pegtop")) == 0)
		{
			m_PegTop = itv->value;
		}
		
		if ((itv->key).compare(string("folder")) == 0)
		{
			m_Folders = itv->value;
		}
		
		if ((itv->key).compare(string("file")) == 0)
		{
			m_Files = itv->value;
		}
		
		if ((itv->key).compare(string("wtime")) == 0)
		{
			m_wTime = itv->value;
		}
		
		if ((itv->key).compare(string("discards")) == 0)
		{
			m_discards = itv->value;
		}
	}
	
	return 0;
}

/************************************
功能：	轮询函数
参数：	无
返回：	0 or 1
************************************/
int CVisionRcm::process_poll(struct pollfd* p)
{
	int err = 0;
	while (1)
	{
		p->revents = 0;
		err = poll(p, 1, -1);
		if (err < 0 && errno == EINTR)
		{
			continue;
		}
		
		close(t_fd);

		if (err && isReady())
		{
			break;
		}
	}
	
	if (m_discards > 0)
	{
		--m_discards;
		return -1;
	}
	
	return 0;
}

/************************************
功能：	是否就绪
参数：	无
返回：	无
************************************/
bool CVisionRcm::isReady()
{
	t_fd = open(DEVICE_SYS_POLL, O_RDWR);
	
	unsigned char install = '\0';
	if (read(t_fd, &install, 1) == 1 && install == '1')
	{
		return true;
	}

	return false;
}

/************************************
功能：	置位0
参数：	无
返回：	无
************************************/
void CVisionRcm::WriteFlg(int fd)
{
	write(fd, "0", 1);
}

/************************************
功能：	生成消息包并发送
参数：	无
返回：	无
************************************/
void CVisionRcm::GenerateMsg()
{	
	GetImuFromCan();
	
	vector<MSG_TAG*>::iterator itv = m_procTag.vPMsgTag.begin();
	for (; itv != m_procTag.vPMsgTag.end(); ++itv)
	{
		/*unsigned char type = *(unsigned char*)(m_regPtr + 0x25 * 4);
		if (type != (*itv)->type)
		{
			continue;
		}*/
	
		VISION_MSG msg;
		
		MSG_TAG* pMsgTag = *itv;
		
		msg.id = pMsgTag->id;
		msg.data.x.size = 0;
		msg.data.ptr = pMsgTag->isBig ? (char*)m_pData : NULL;
		
		int err = 0;
		while (NULL != pMsgTag)
		{
			if (-1 == (this->*(pMsgTag->pf))(&msg, pMsgTag->begin_pos, pMsgTag->off_set))
			{
				err = -1;
				break;
			}
			
			pMsgTag = pMsgTag->next;
		}

		if (0 == err && -1 == SendMsg(&msg))
		{
			LOGE("send msg %ld err. %s : %d\n", msg.id, __FILE__, __LINE__);
		}
	}
}

/************************************
功能：	FPGA预处理
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::Preprocess4Fpga()
{
	// 加载FPGA前，设置未初始化
	SetStatusCode(ERR_FPGA_UNINITED);
	
	// 加载rbf配置文件
	if (0 != WriteRbf())
	{
		return -1;
	}
	
	// 等待fpga加载完成
	Wait4FPGAReady();
	
	// 写fpga参数
	if (0 != WriteParameter())
	{
		return -1;
	}
	
	// 写CMS参数
	if (0 != WriteCmos())
	{
		return -1;
	}
	
	// 恢复状态
	SetStatusCode(-ERR_FPGA_UNINITED);
	
	// 使能FPGA
	EnableFPGA();
	
	return 0;
}

/************************************
功能：	加载FPGA文件
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::WriteRbf()
{
	LOGW("Begin writing rbf. %s : %d\n", __FILE__, __LINE__);
	
	int rfd = open(DEVICE_RBF_FILE, O_RDONLY);
	if (rfd < 0)
	{
		LOGE("Open file %s err. %s : %d\n", DEVICE_RBF_FILE, __FILE__, __LINE__);
		return -1;
	}
	
	int wfd = open(DEVICE_RBF, O_WRONLY);
	if (wfd < 0)
	{
		LOGE("Open file %s err. %s : %d\n", DEVICE_RBF, __FILE__, __LINE__);
		return -1;
	}
	
	char szBuf[1024] = {0};
	
	int err = 0;
	while((err = read(rfd, szBuf, 1024)) > 0)
	{
		write(wfd, szBuf, err);
	}
	
	close(rfd);
	close(wfd);
	
	LOGW("Write rbf end. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	等待FPGA配置完成
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::Wait4FPGAReady()
{
	LOGW("Begin wait for fpga ready. %s : %d\n", __FILE__, __LINE__);
	
	unsigned int uTestValue = 0x55555555;
	
	unsigned int* pTestReg = (unsigned int*)(m_regPtr + 0x20);
	while (IsRunning())
	{
		*pTestReg = uTestValue;
		
		usleep(100000);
		
		if (~uTestValue == *pTestReg)
		{
			break;
		}
	}
	
	unsigned int * pInitialStatus = (unsigned int*)(m_regPtr + 0x18);
	while (IsRunning())
	{
		if (*pInitialStatus & 0x80000000)
		{
			break;
		}
		
		usleep(500000);
	}
	
	LOGW("Fpag are ready to config. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	写算法参数
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::WriteParameter()
{
	LOGW("Begin writing parameter. %s : %d\n", __FILE__, __LINE__);
	
	FILE* pf = fopen(MAP_HEX_FILE, "r");
	if (NULL == pf)
	{
		LOGE("Open file %s err. %s : %d\n", MAP_HEX_FILE, __FILE__, __LINE__);
		return -1;
	}
	
	unsigned int hex = 0;
	unsigned int *pHex[2] = {(unsigned int*)(m_regPtr + 0xC0)
		, (unsigned int*)(m_regPtr + 0xC4)};
	
	unsigned int i = 0;
	while (EOF != fscanf(pf, "%x", &hex))
	{
		*pHex[i % 2] = hex;
		
		++i;
	}
	
	fclose(pf);
	
	LOGW("Write fpga alg parameter success. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	FPGA预处理
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionRcm::WriteCmos()
{
	LOGW("Begin writing cmos. %s : %d\n", __FILE__, __LINE__);
	
	FILE* pf = fopen(COMOS_FILE, "r");
	if (NULL == pf)
	{
		LOGE("Open file %s err. %s : %d\n", COMOS_FILE, __FILE__, __LINE__);
		return -1;
	}
	
	unsigned int hex = 0;
	unsigned int *pHex = (unsigned int*)(m_regPtr + 0x20 * 4);
	
	while (EOF != fscanf(pf, "%x", &hex))
	{
		*pHex = hex;
		
		usleep(5000);
	}
	
	fclose(pf);
	
	LOGW("Write cmos succeed. %s : %d\n", __FILE__, __LINE__);

	return 0;
}

void CVisionRcm::EnableFPGA()
{
	unsigned int* pHpsFpgaTringN = (unsigned int*)(m_regPtr + 0x1C);
	
	*pHpsFpgaTringN &= 0xFFFFFFFE;
}

void CVisionRcm::DisableFPGA()
{
	unsigned int* pHpsFpgaTringN = (unsigned int*)(m_regPtr + 0x1C);
	
	*pHpsFpgaTringN |= 0x00000001;
}

void CVisionRcm::EnableSonar()
{
	unsigned int* pSonar = (unsigned int*)(m_regPtr + 0x50 * 4);
	
	*pSonar = 0xFFFFFFFF;
}

void CVisionRcm::DisableSonar()
{
	unsigned int* pSonar = (unsigned int*)(m_regPtr + 0x50 * 4);
	
	*pSonar |= 0x00000001;
}

void CVisionRcm::GetImuFromCan()
{
	IMU_DATA* pImu = &m_imu;
	
	int len = (int)sizeof(IMU_DATA);
	
	// IMU
	CHF::GetContent(m_can0, (char*)pImu, &len);
	
	// 判断四元素正确性
	float q = pImu->q0 * pImu->q0 + pImu->q1 * pImu->q1 + pImu->q2 * pImu->q2 + pImu->q3 * pImu->q3;
	if (q > 1.05 || q < 0.95)
	{
		// 未设置错误码时，需设置错误码
		if (!m_bImuErr)
		{
			SetStatusCode(ERR_IMU_UNRECEIVE);
			m_bImuErr = true;
		}
	}
	else if (m_bImuErr)
	{
		// 未恢复时，需恢复
		SetStatusCode(-ERR_IMU_UNRECEIVE);
		m_bImuErr = false;
	}
	
	// 超声波 
	pImu->sonar = 0;
	if (m_Sonar)
	{	
		unsigned char* pSonar = (unsigned char*)(m_regPtr + 0x51 * 4);
		unsigned char uCheckSum = *pSonar + *(pSonar + 1) + 0xFF;
		if (uCheckSum == *(pSonar + 2))
		{
			unsigned short* pDistance = (unsigned short*)pSonar;
			
			pImu->sonar = *pDistance * 0.001f;
		}
	}
	
	// 陀螺仪
	if (m_PegTop)
	{
		U_4Q u_4Q;
		
		u_4Q.nq = *(int*)(m_regPtr + 0x40 * 4);
		pImu->q0 = u_4Q.q;
		
		u_4Q.nq = *(int*)(m_regPtr + 0x41 * 4);
		pImu->q1 = u_4Q.q;
		
		u_4Q.nq = *(int*)(m_regPtr + 0x42 * 4);
		pImu->q2 = u_4Q.q;
		
		u_4Q.nq = *(int*)(m_regPtr + 0x43 * 4);
		pImu->q3 = u_4Q.q;
	}
}

#ifdef _RUN_SMALL_ALG_

void CVisionRcm::RunAlgTask()
{
	// 避障算法任务
	RECTIFIED_IMG rec_info;
	
	// 准备避障数据
	Prepare4Bm(&rec_info);
	
	char szOut[256] = {0};
	int len = 0;
	unsigned alarm = 0;
	
	// 运行避障算法
	RunBm(&rec_info, szOut, len, alarm);
	
	SendCanData(m_can1, 0x608, szOut, len);
}

#endif

void CVisionRcm::Prepare4Bm(RECTIFIED_IMG* pRecInfo)
{
	if (NULL == pRecInfo)
	{
		return;
	}
	
	// 深度图
	pRecInfo->pDisparity = (char*)(m_ptr + m_offset_d);
	
	// 矫正图像
	pRecInfo->plImg = (char*)(m_ptr + m_offset_l);
	pRecInfo->prImg = (char*)(m_ptr + m_offset_r);
	
	// 姿态包
	pRecInfo->imu = m_imu;
	
	// 曝光时间
	unsigned* pHex = (unsigned*)(m_regPtr + 0x107 * 4);
	pRecInfo->eTime[0] = *pHex * 20;
	
	pHex = (unsigned*)(m_regPtr + 0x108 * 4);
	pRecInfo->eTime[1] = *pHex * 20;

	pHex = (unsigned*)(m_regPtr + 0x109 * 4);
	pRecInfo->eTime[2] = *pHex * 20;	
	
	pHex = (unsigned*)(m_regPtr + 0x10a * 4);
	pRecInfo->eTime[3] = *pHex * 20;
	
	// 超声波, 待增加	
	
	return;
}

void CVisionRcm::OutTime(FILE* pf)
{
	if (NULL == pf)
	{
		return;
	}
	
	struct timeval t;
	gettimeofday(&t, NULL);
	
	unsigned ut = t.tv_sec * 1000 + t.tv_usec / 1000;
	
	fprintf(pf, "%06d	%08d\n", m_index, ut);
	
	fflush(pf);
}


