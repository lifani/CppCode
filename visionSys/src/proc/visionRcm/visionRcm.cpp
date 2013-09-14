#include <common_interface.h>
#include <mt/mt.h>
#include <libproperty/cutils/properties.h>

#include "visionRcm.h"

extern "C" int __system_properties_init(void);

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
, st_fd(0)
, m_fd(0)
, m_ptr(NULL)
, m_sendFlg(0)
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
		return -1;
	}
	
	// 线程标志
	m_bRunning = true;
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	
	__system_properties_init();
	
	// 轮休描述符
	if ((st_fd = open(DEVICE_SYS_POLL, O_RDWR)) < 0)
	{
		return -1;
	}
	
	// 文件映射存储
	if (-1 == InitMMap())
	{
		return -1;
	}
	
	return 0;
}

void CVisionRcm::Run()
{	
	VELOCITY_DATA tVelocity;
	RECTIFIED_IMG tRectified;
	FEEDBACK_DATA tFeedback;
	
	unsigned int size = 0;
	
	struct pollfd p_fd;
	
	// 等待fpga加载完成
	Wait4FPGAReady();
	
	// 写fpga参数
	WriteParameter();

	while (m_bRunning)
	{
		p_fd.fd = st_fd;
		p_fd.events = POLLERR | POLLHUP;
		
		// 调用轮询函数
		process_poll(&p_fd);

		// 读取点云数据, 读取图像数据
		if (-1 == ReadVelocityData(tVelocity)
			|| -1 == ReadRectifiedImg(tRectified))
		{
			writeFlg(st_fd);
			continue;
		}
		
		// 写标志
		writeFlg(st_fd);
		
		// 获取IMU数据
		
		// 发送点云数据
		size = sizeof(VELOCITY_DATA);
		
		int len = 0;
		if ((len = SendData(string("visionVelocity"), (char*)&tVelocity, &size)) == -1)
		{
			cout << "send cloud error." << endl;
		}
		
		// 隔帧发送图像数据
		
		m_sendFlg ^= 0x01;
		if (m_sendFlg == 1)
		{
			size = sizeof(RECTIFIED_IMG);
			if (SendData(string("visionBm"), (char*)&tRectified, &size) == -1)
			{
				cout << "send img error." << endl;
			}
		}
	}
}

void CVisionRcm::Run1()
{
	while(m_bRunning)
	{
		// 接收IMU数据
		
		sleep(2);
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

void CVisionRcm::Wait4FPGAReady()
{
	int fd = open(FPGA_READY, O_RDONLY);
	if (fd < 0)
	{
		printf("open fpga_ready error...\n");	
		return;
	}
	
	char data;
	while(m_bRunning)
	{
		int err = read(fd, &data, 1);
		if ((err > 0) && (data == '1')) 
		{
			printf("FPGA Ready to go...\n");
			break;
		}
	}
	
	close(fd);
}

void CVisionRcm::writeFlg(int fd)
{
	write(fd, "0", 1);
}

int CVisionRcm::WriteParameter(int type)
{
	switch (type)
	{
	case 0:
		property_set("sys.fpga_parameter.config", "0");
		break;
	case 1:
		property_set("sys.fpga_parameter.config", "1");
		break;
	case 2:
		property_set("sys.fpga_parameter.config", "2");
		break;
	default:
		break;
	}
	
	unsigned char* pData = new unsigned char[2 * 1024 * 1024];
	if (NULL == pData)
	{
		return -1;
	}
	
	FILE* pf = fopen(MAP_HEX_FILE, "r");
	if (NULL == pf)
	{
		delete []pData;
		return false;
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
		memcpy(m_ptr, pos, len);
		
		i -= len;
		pos += len;
		
		if (len != write(m_fd, NULL, len))
		{
			cout << "write prameter error." << endl;
			
			delete []pData;
			return -1;
		}
	}
	delete []pData;
	
	// enable fpga
	property_set("sys.fpga_parameter.config", "3");
	property_set("sys.fpga.config", "0");
	
	return 0;
}

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
	
	return 0;
}

int CVisionRcm::ReadRectifiedImg(RECTIFIED_IMG& tRectified)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	memcpy(tRectified.lImg, (char*)(m_ptr + MAX_CLOUD_SIZE * 2), IMG_SIZE);
	memcpy(tRectified.rImg, (char*)(m_ptr + MAX_CLOUD_SIZE * 2 + IMG_SIZE), IMG_SIZE);
	
	return 0;
}

