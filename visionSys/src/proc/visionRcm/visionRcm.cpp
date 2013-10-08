#include <common_interface.h>
#include <mt/mt.h>
#include <libproperty/cutils/properties.h>

#include "visionRcm.h"

extern "C" int __system_properties_init(void);

pthread_mutex_t CVisionRcm::imu_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CVisionRcm::can_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CVisionRcm::can_ready = PTHREAD_COND_INITIALIZER;

CCommonInterface* CreateInstance(const char* ppname, const char* pname)
{
	if (NULL == ppname || NULL == pname)
	{
		return NULL;
	}
	
	CCommonInterface* p = new CVisionRcm(ppname, pname);
	
	return p;
}

CVisionRcm::CVisionRcm(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, m_bRunning(true)
, m_isImuReady(false)
, st_fd(0)
, m_fd(0)
, m_ptr(NULL)
, m_can0(NULL)
, m_FetchPos(-1)
, m_StorePos(0)
, m_can_v_fetch(-1)
, m_can_v_store(0)
, m_can_b_fetch(-1)
, m_can_b_store(0)
, m_index(0)
, m_sndflg(0)
{
}

CVisionRcm::~CVisionRcm()
{
}

int CVisionRcm::Active()
{
	// 使用父类的Active
	if (CBaseVision::Active() == -1)
	{
		LOGE("%s's basic class active error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
		return -1;
	}
	
	// 线程标志
	m_bRunning = true;
	
	// can
	m_can0 = CreateCanInterface("can0");
	if (NULL == m_can0)
	{
		LOGE("create can0 error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 设置normal
	if (!m_can0->SetFilter(CAN_ID_NORMAL, CMD_CODE_NORMAL))
	{
		LOGE("can0 set filter error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	NormalMode tNormalMode;

	m_can0->Read((char*)&tNormalMode, sizeof(NormalMode));
	m_can0->SetKey(tNormalMode.key);
	
	LOGW("IMU key = %02x. %s : %d\n", tNormalMode.key, __FILE__, __LINE__);
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	RegisterPthread(&CBaseVision::Run2);
	
	__system_properties_init();
	
	// 轮询描述符
	if ((st_fd = open(DEVICE_SYS_POLL, O_RDWR)) < 0)
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
	
	LOGW("%s active success. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
	
	return 0;
}

void CVisionRcm::Run()
{	
	VELOCITY_DATA tVelocity;
	RECTIFIED_IMG tRectified;
	
	unsigned int size = 0;
	
	struct pollfd p_fd;
	
	// 等待fpga配置完成
	Wait4FPGAReady(FPGA_CONIFG_OK);
	
	// 等待fpga加载完成
	Wait4FPGAReady(FPGA_READY);
	
	// 写fpga参数
	WriteParameter();
	
	// 写CMS参数
	WriteCmos();
	
	property_set("sys.fpga.config", "0");
	
	unsigned last_cnt_v = 0;
	unsigned last_cnt_b = 0;

	while (m_bRunning)
	{
		p_fd.fd = st_fd;
		p_fd.events = POLLERR | POLLHUP;
		
		// 调用轮询函数
		process_poll(&p_fd);

		// 读取点云数据, IMU数据, 读取图像数据
		if (-1 == ReadVelocityData(tVelocity)
			|| -1 == ReadRectifiedImg(tRectified))
		{
			writeFlg(st_fd);
			continue;
		}
		
		m_index++;
		
		// 写标志
		writeFlg(st_fd);
		
		// 发送点云数据
		size = sizeof(VELOCITY_DATA);
		
		if (SendData(string("visionVelocity"), (char*)&tVelocity, &size) == -1)
		{
			//cout << "send cloud error." << endl;
		}
		
		// 发送图像数据
		m_sndflg ^= 0x01;
		if (0x01 == m_sndflg)
		{
			size = sizeof(RECTIFIED_IMG);
			if (SendData(string("visionBm"), (char*)&tRectified, &size) == -1)
			{
				//cout << "send img error." << endl;
			}
		}
		
		pthread_mutex_lock(&can_lock);
		
		FEEDBACK_DATA* pFeedBack = (FEEDBACK_DATA*)&tVelocity;
		if (pFeedBack->flg == IS_VELOCITY)
		{
			if (0 == last_cnt_v || last_cnt_v != pFeedBack->cnt)
			{
				// 入发送队列
				memcpy((char*)&can_v[m_can_v_store++], pFeedBack->data, sizeof(CAN_VELOCITY_DATA));

				if (QUEUE_SIZE == (unsigned int)m_can_v_store)
				{
					m_can_v_store = 0;
				}
				
				m_can_v_fetch++;
				if (QUEUE_SIZE == (unsigned int)m_can_v_fetch)
				{
					m_can_v_fetch = 0;
				}
				
				last_cnt_v = pFeedBack->cnt;
			}
		}

		if (0x01 == m_sndflg)
		{
			pFeedBack = (FEEDBACK_DATA*)&tRectified;
			if (pFeedBack->flg == IS_BM)
			{
				if (0 == last_cnt_b || last_cnt_b != pFeedBack->cnt)
				{
					// 入发送队列
					memcpy((char*)&can_b[m_can_b_store++], pFeedBack->data, sizeof(CAN_BM_DATA));
				
					if (QUEUE_SIZE == (unsigned int)m_can_b_store)
					{
						m_can_b_store = 0;
					}
					
					m_can_b_fetch++;
					if (QUEUE_SIZE == (unsigned int)m_can_b_fetch)
					{
						m_can_b_fetch = 0;
					}
					
					last_cnt_b = pFeedBack->cnt;
				}
			}
		}
		
		pthread_mutex_unlock(&can_lock);
		pthread_cond_signal(&can_ready);
	}
}

void CVisionRcm::Run1()
{	
	// 设置atti
	if (!m_can0->SetFilter(CAN_ID_ATTI, CMD_CODE_ATTI))
	{
		return;
	}
	
	imu_body imu;
	while(m_bRunning)
	{
		// 接收IMU数据
		m_can0->Read((char*)&imu, sizeof(imu_body), 1);
		
		// 加入队列
		pthread_mutex_lock(&imu_lock);
		
		imu_body* pImu = &m_imu_body[m_StorePos];
		memcpy((char*)pImu, (char*)&imu, sizeof(imu_body));
		
		m_StorePos++;
		if (QUEUE_SIZE == (unsigned int)m_StorePos)
		{
			m_StorePos = 0;
		}
		
		m_FetchPos++;
		if (QUEUE_SIZE == (unsigned int)m_FetchPos)
		{
			m_FetchPos = 0;
		}
		
		m_isImuReady = true;
		
		pthread_mutex_unlock(&imu_lock);
		
		usleep(5000);
	}
}

void CVisionRcm::Run2()
{
	CAN_VELOCITY_DATA* pV = NULL;
	CAN_BM_DATA* pb = NULL;
	
	int last_fetch_v = -1;
	int last_fetch_b = -1;
	while (m_bRunning)
	{
		pV = NULL;
		pb = NULL;
		
		// 取数据
		pthread_mutex_lock(&can_lock);
		while (m_can_v_fetch == last_fetch_v && m_can_b_fetch == last_fetch_b)
		{
			 pthread_cond_wait(&can_ready, &can_lock);
		}
		
		// 取测速数据
		if (m_can_v_fetch != last_fetch_v)
		{
			pV = (CAN_VELOCITY_DATA*)&can_v[m_can_v_fetch];
			
			last_fetch_v = m_can_v_fetch;
		}

		// 取避障数据
		if (m_can_b_fetch != last_fetch_b)
		{
			pb = (CAN_BM_DATA*)&can_b[m_can_b_fetch];

			last_fetch_b = m_can_b_fetch;
		}
		
		pthread_mutex_unlock(&can_lock);
		
		// 发送测速数据
		if (NULL != pV)
		{
			m_can0->SetProtocal(true);
			m_can0->Write((char*)pV, sizeof(CAN_VELOCITY_DATA), 0x091, 0x1005);
		}
		
		// 发送避障数据
		if (NULL != pb)
		{
			m_can0->SetProtocal(false);
			m_can0->Write((char*)pb, sizeof(CAN_BM_DATA), 0x70a, 0x1005);
		}
	}
}

int CVisionRcm::Deactive()
{
	m_bRunning = false;
	
	// 关闭轮询文件描述符
	close(st_fd);
	
	// 注销文件映射存储
	if (NULL != m_ptr)
	{
		munmap(m_ptr, MMAP_SIZE);
		close(m_fd);
	}
	
	// disable fpga
	property_set("sys.fpga.config", "1");
	
	return CBaseVision::Deactive();
}

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

// 轮询函数
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
		
		close(st_fd);

		if (err && isReady())
		{
			break;
		}
	}
}

// 是否就绪
bool CVisionRcm::isReady()
{
	st_fd = open(DEVICE_SYS_POLL, O_RDWR);
	
	unsigned char install = '\0';
	int err = read(st_fd, &install, 1);
	if (err == 1 && install == '1')
	{
		return true;
	}

	return false;
}

// 等待fpga完成
void CVisionRcm::Wait4FPGAReady(const char* file)
{
	if (NULL == file)
	{
		return;
	}

	char data;
	while(m_bRunning)
	{
		int fd = open(file, O_RDONLY);
		if (fd < 0)
		{
			LOGE("open fpga_ready error. %s : %d\n", __FILE__, __LINE__);
			return;
		}
	
		int err = read(fd, &data, 1);
		if ((err > 0) && (data == '1')) 
		{
			LOGW("FPGA Ready to go. %s : %d\n", __FILE__, __LINE__);
			break;
		}
		
		close(fd);
		
		usleep(500000);
	}
}

void CVisionRcm::writeFlg(int fd)
{
	write(fd, "0", 1);
}

int CVisionRcm::WriteParameter(int type)
{	
	unsigned char* pData = new unsigned char[2 * 1024 * 1024];
	if (NULL == pData)
	{
		LOGE("create pdata error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	FILE* pf = fopen(MAP_HEX_FILE, "r");
	if (NULL == pf)
	{
		LOGE("open file %s error. %s : %d\n", MAP_HEX_FILE, __FILE__, __LINE__);
		
		delete []pData;
		return -1;
	}

	unsigned int hex = 0;
	unsigned int i = 0;

	while (EOF != fscanf(pf, "%x", &hex))
	{
		unsigned int* pHex = (unsigned int*)((unsigned char*)pData + i);
		*pHex = hex;
		
		i += 4;
	}

	fclose(pf);
	
	unsigned char* pos = pData;

	while (i > 0 && NULL != pos)
	{
		int len = i > IMG_SIZE ? IMG_SIZE : i;
		memset(m_ptr, 0x0, len);
		memcpy(m_ptr, pos, len);
		
		i -= len;
		pos += len;
		
		property_set("sys.fpga_parameter.config", "0");
		if (len != write(m_fd, NULL, len))
		{
			LOGE("write prameter error. %s : %d\n", __FILE__, __LINE__);
			
			delete []pData;
			return -1;
		}
		property_set("sys.fpga_parameter.config", "3");
	}
	delete []pData;
	
	LOGW("write fpga alg parameter success. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

int CVisionRcm::WriteCmos()
{		
	unsigned char pData[1024] = {0};
	
	FILE* pf = fopen(COMOS_FILE, "r");
	if (NULL == pf)
	{
		LOGE("open file %s error. %s : %d\n", COMOS_FILE, __FILE__, __LINE__);
		return -1;
	}

	unsigned int hex = 0;
	unsigned int i = 0;

	while (EOF != fscanf(pf, "%x", &hex))
	{
		unsigned int* pHex = (unsigned int*)((unsigned char*)pData + i);
		*pHex = (unsigned short)hex;
		
		i += 2;
	}

	fclose(pf);
	
	unsigned char* pos = pData;

	while (i > 0 && NULL != pos)
	{
		int len = i > IMG_SIZE ? IMG_SIZE : i;
		memset(m_ptr, 0x0, len);
		memcpy(m_ptr, pos, len);
		
		i -= len;
		pos += len;
		
		property_set("sys.fpga_parameter.config", "2");
		if (len != write(m_fd, NULL, len))
		{
			LOGE("write comos error. %s : %d\n", __FILE__, __LINE__);
			return -1;
		}
		property_set("sys.fpga_parameter.config", "3");
	}
	
	LOGW("write fpga cmos parameter success. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

// 读取velocity数据
int CVisionRcm::ReadVelocityData(VELOCITY_DATA& tVelocity)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	tVelocity.lcnt = *(unsigned short*)m_ptr;
	tVelocity.rcnt = *(unsigned short*)(m_ptr + MAX_CLOUD_SIZE);
	
	if (tVelocity.lcnt > MAX_CLOUD_CNT || tVelocity.rcnt > MAX_CLOUD_CNT)
	{
		return -1;
	}
	
	memcpy(tVelocity.lCloud, m_ptr + HEAD_SIZE, tVelocity.lcnt * POINT_LEN);
	memcpy(tVelocity.rCloud, m_ptr + MAX_CLOUD_SIZE + HEAD_SIZE, tVelocity.lcnt * POINT_LEN);
	
	// 获取IMU数据
	GetIMU(tVelocity.imu);
	
	tVelocity.index = m_index;
	
	return 0;
}

// 读取矫正图像
int CVisionRcm::ReadRectifiedImg(RECTIFIED_IMG& tRectified)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	memcpy(tRectified.lImg, (char*)(m_ptr + MAX_CLOUD_SIZE * 2), IMG_SIZE);
	memcpy(tRectified.rImg, (char*)(m_ptr + MAX_CLOUD_SIZE * 2 + IMG_SIZE), IMG_SIZE);
	
	tRectified.index = m_index;
	
	return 0;
}

// 获取IMU数据
void CVisionRcm::GetIMU(IMU& imu)
{	
	pthread_mutex_lock(&imu_lock);
	
	if (!m_isImuReady)
	{
		pthread_mutex_unlock(&imu_lock);
		return;
	}
	
	imu_body* p = &m_imu_body[m_FetchPos];
	if (NULL != p)
	{
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
	}
	
	pthread_mutex_unlock(&imu_lock);
}

