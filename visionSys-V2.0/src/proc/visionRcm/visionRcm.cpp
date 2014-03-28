/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "visionRcm.h"

#undef LOG_TAG
#define LOG_TAG "VISION_RCM"

BEGAIN_MESSAGE_MAP(CVisionRcm, CBaseVision)
	ON_COMMAND(VELOCITY_BACK, &CVisionRcm::ProcessVelocityMsg)
	ON_COMMAND(RCM_LOG, &CVisionRcm::OutLog)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionRcm, CBaseVision)
	ON_TIMER(100000, &CVisionRcm::SendCanData)
END_TIMER_MAP()

DEFINE_CREATE_INSTANCE(CVisionRcm)

CVisionRcm::CVisionRcm(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, t_fd(0)
, m_fd(0)
, m_regFd(0)
, m_can0(0)
, m_subs(0)
, m_Sonar(0)
, m_Naza(0)
, m_Num(0)
, m_PegTop(0)
, m_Folders(0)
, m_Files(0)
, m_sumSize(0)
, m_ptr(0)
, m_regPtr(0)
, m_pData(0)
, m_qCtrl(sizeof(CAN_VELOCITY_DATA), 10, false)
, m_work(false)
{
}

CVisionRcm::~CVisionRcm()
{
}

/************************************
���ܣ�	VisionRcm���������ɳ�ʼ������
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CVisionRcm::ActiveImp()
{
	if (0 != Initialize())
	{
		LOGE("Initialize fail. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}

	// ע���̺߳���
	REGISTER_THREAD(&CVisionRcm::TransData);
	
	// ע����Ϣ���ɺ���
	REGISTER_MSG_FUNC("GetDataFromFpga", &CVisionRcm::GetDataFromFpga);
	
	LOGW("VisionRcm actived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
���ܣ�	ʵ�ָ�����Դ���ͷ�
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CVisionRcm::DeactiveImp()
{
	DisableFPGA();
	
	CHF::Destroy();
	
	// �ر���ѯ�ļ�������
	close(t_fd);
	
	// ע���ļ�ӳ��洢
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
	
	LOGW("VisionRcm deactived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
���ܣ�	���̣߳�������������������ݶ�ȡ�����ݡ�
������	��
���أ�	��
************************************/
void CVisionRcm::TransData()
{
	struct pollfd p_fd;

	while(IsRunning())
	{
		p_fd.fd = t_fd;
		p_fd.events = POLLERR | POLLHUP;
	
		// ������ѯ����
		process_poll(&p_fd);
		
		// ������Ϣ������
		GenerateMsg();
		
		// ����ѯ��־λ��0
		WriteFlg(t_fd);
	}
}

/************************************
���ܣ�	���ٽ����Ϣ����ӿ�
������	��
���أ�	��
************************************/
void CVisionRcm::ProcessVelocityMsg(VISION_MSG* pMsg)
{	
	if (NULL != pMsg)
	{
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)pMsg->data.buf;

		m_qCtrl.push((char*)p);
	}
}

/************************************
���ܣ�	ͨ��CAN�ڷ�������
������	��
���أ�	�� 
************************************/
void CVisionRcm::SendCanData()
{
	CAN_SNT_DATA tSntData;

	CAN_VELOCITY_DATA data;
	if (m_qCtrl.pop((char*)&data) == 0)
	{	
		tSntData.can_id = 0x095;
		tSntData.data = (char*)&data;
		
		CHF::SetContent(m_can0, (char*)&tSntData, sizeof(CAN_VELOCITY_DATA));
	}
}

int CVisionRcm::GetImu(VISION_MSG* pMsg, int beginPos, int offset)
{
	return 0;
}

int CVisionRcm::GetDataFromFpga(VISION_MSG* pMsg, int beginPos, int offset)
{
	if (NULL != pMsg->data.ptr)
	{
		memcpy(pMsg->data.ptr + pMsg->data.size, m_ptr + beginPos, offset);
		pMsg->data.size += offset;
	}
	
	return 0;
}

int CVisionRcm::GetVCtrl(VISION_MSG* pMsg, int beginPos, int offset)
{
	return 0;
}

/************************************
���ܣ�	ʵ�ָ�����Դ�ĳ�ʼ��
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CVisionRcm::Initialize()
{
	m_pData = new unsigned char[MSG_MEM_SIZE / 2];
	if (NULL == m_pData)
	{
		LOGE("malloc memory err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}

	// ��ʼ��ѡ��
	InitOption();
	
	// ��ʼ�����й�����
	if (0 != m_qCtrl.Initialize())
	{
		LOGE("queue contrl module init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// �õ�������
	m_can0 = CHF::FD(HF_CAN0, m_Naza);
	if (-1 == m_can0)
	{
		LOGE("Get can interface err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	m_subs = CHF::FD(HF_COM);
	if (-1 == m_subs)
	{
		LOGE("get com interface err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// ���CHF�ĳ�ʼ������
	if (-1 == CHF::Initialize())
	{
		LOGE("CHF init err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("CHF init succeed. %s : %d\n", __FILE__, __LINE__);
	
	// ��ѯ������
	if ((t_fd = open(DEVICE_SYS_POLL, O_RDWR)) < 0)
	{
		LOGE("open poll fd error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// �ļ�ӳ��洢
	if (-1 == InitMMap())
	{
		LOGE("mmap error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("map fpga memory succeed. %s : %d\n", __FILE__, __LINE__);
	
	// ӳ��Ĵ���
	if (-1 == InitRegMMap())
	{
		LOGE("map register err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("map register succeed. %s : %d\n", __FILE__, __LINE__);
	
	// FPGAԤ����
	if (-1 == Preprocess4Fpga())
	{
		LOGE("Prepare for fpga err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	LOGW("Prepare for fpga succeed. %s : %d\n", __FILE__, __LINE__);
	
	// ʹ�ܳ�����
	if (m_Sonar)
	{
		EnableSonar();
	}
	
	return 0;
}

/************************************
���ܣ�	�ļ�ӳ��洢
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
���ܣ�	ӳ��Ĵ�����ַ
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
���ܣ�	��ʼ������ѡ��
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
	}
	
	return 0;
}

/************************************
���ܣ�	��ѯ����
������	��
���أ�	��
************************************/
void CVisionRcm::process_poll(struct pollfd* p)
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
}

/************************************
���ܣ�	�Ƿ����
������	��
���أ�	��
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
���ܣ�	��λ0
������	��
���أ�	��
************************************/
void CVisionRcm::WriteFlg(int fd)
{
	write(fd, "0", 1);
}

/************************************
���ܣ�	������Ϣ��������
������	��
���أ�	��
************************************/
void CVisionRcm::GenerateMsg()
{	
	vector<MSG_TAG*>::iterator itv = m_procTag.vPMsgTag.begin();
	for (; itv != m_procTag.vPMsgTag.end(); ++itv)
	{
		unsigned char type = *(unsigned char*)(m_regPtr + 0x25 * 4);
		if (type != (*itv)->type)
		{
			continue;
		}
	
		VISION_MSG msg;
		
		MSG_TAG* pMsgTag = *itv;
		
		msg.id = pMsgTag->id;
		msg.data.size = 0;
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
���ܣ�	FPGAԤ����
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CVisionRcm::Preprocess4Fpga()
{
	// ����rbf�����ļ�
	if (0 != WriteRbf())
	{
		return -1;
	}
	
	// �ȴ�fpga�������
	Wait4FPGAReady();
	
	// дfpga����
	if (0 != WriteParameter())
	{
		return -1;
	}
	
	// дCMS����
	if (0 != WriteCmos())
	{
		return -1;
	}
	
	// ʹ��FPGA
	EnableFPGA();
	
	return 0;
}

/************************************
���ܣ�	����FPGA�ļ�
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
���ܣ�	�ȴ�FPGA�������
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
���ܣ�	д�㷨����
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
���ܣ�	FPGAԤ����
������	��
���أ�	�ɹ� 0��ʧ�� -1
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

/************************************
���ܣ�	��ȡIMU����
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
/*int CVisionRcm::GetImu(IMU_DATA* pImu)
{
	int len = (int)sizeof(IMU_DATA);
	
	// IMU
	CHF::GetContent(m_can0, (char*)pImu, &len);
	
	// ������ 
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
	
	// ������
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
	
	return 0;
}*/

