#include "visionProcess.h"
#include "tools.h"
#include "visionMonitor.h"
#include "visionImu.h"
#include "visionStore.h"
#include "../VO/demon.h"

static VisionNode* pQHead = NULL;
static VisionNode* pQTail = NULL;

static void* ptrData = NULL;
static int fd = 0;
static int st_fd = 0;

static pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t qready = PTHREAD_COND_INITIALIZER;

static bool process_running = true;
static bool read_running = true;

// 视觉处理线程入口函数
 void* process_vision(void* arg)
{
	VisionNode* pNode = NULL;
	
	while (process_running)
	{
		pthread_mutex_lock(&qlock);
		while( NULL == pQHead )
		{
			pthread_cond_wait(&qready, &qlock);
		}

		pNode       = pQHead;
		pQHead      = pQHead->next;
		pNode->next = NULL;

		// 视觉算法处理
		Run(pNode->lImage, pNode->rImage, (char*)&pNode->imu);
		
#ifndef NO_STORE
		if (0x08 && BIT_MASK)
		{
			append_vision_queue(pNode);
		}
#endif
		// 处理结束后释放空间
		if (NULL != pNode)
		{
			pNode->destroy(pNode);
			pNode = NULL;
		}

		pthread_mutex_unlock(&qlock);
	}
	
	NotifyExit(PROC_EXIT);

	return NULL;
}

// 图像数据入处理队列函数
void enter_vision_queue(VisionNode* pNode)
{
	if (NULL == pNode)
	{
		return;
	}

	pthread_mutex_lock(&qlock);

	if (NULL == pQHead)
	{
		pQHead = pNode;
		pQTail = pQHead;
	}
	else
	{
		pQTail->next = pNode;
		pQTail = pQTail->next;
	}

	pthread_mutex_unlock(&qlock);
	pthread_cond_signal(&qready);
}

void* InitMMap()
{
	fd = open(DEVICE_FILENAME, O_RDWR);
	if (fd < 0)
	{
		Writelog(LOG_ERR, "Open fpga file failed.", __FILE__, __LINE__);
		return 0;
	}
	
	ptrData = mmap(0, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (NULL == ptrData)
	{
		Writelog(LOG_ERR, "mmap failed.", __FILE__, __LINE__);
		return 0;
	}
	
	return ptrData;
}

void DestoryMMap()
{
	munmap(ptrData, MMAP_SIZE);
	close(fd);
}

// 读取图像数据线程入口函数
void* read_vision(void* arg)
{
	// 算法注册
	Register();

	// 初始化队列，必选项
	if (!VisionNode::InitMemory())
	{
		Writelog(LOG_ERR, "Initialize memory fail.", __FILE__, __LINE__);
		NotifyExit(READ_EXIT);
		return NULL;
	}
	
	VisionNode* pNode = NULL;

	// 轮询、获取图片数据
	st_fd = open(DEVICE_SYS_POLL, O_RDWR);
	if (st_fd < 0)
	{
		Writelog(LOG_ERR, "Open poll file failed.", __FILE__, __LINE__);
		NotifyExit(READ_EXIT);
		return NULL;
	}

	struct pollfd p_fd;

	while (read_running)
	{
		pNode = VisionNode::Instance();

		p_fd.fd = st_fd;
		p_fd.events = POLLERR | POLLHUP;
		
		// 调用轮询函数
		process_poll(&p_fd);

		// 读取图像数据
		if (!ReadImg(pNode))
		{
			writeFlg(p_fd.fd);
			VisionNode::destroy(pNode);
			
			Writelog(LOG_ERR, "Read img fail.", __FILE__, __LINE__);
				
			continue;
		}
			
		writeFlg(st_fd);

#ifndef NO_IMURECV
		
		// 读取IMU数据
		GetIMU(pNode->imu);
#endif

		// 入队列
		enter_vision_queue(pNode);
		pNode = NULL;
	}
	
	close(st_fd);
	
	pthread_exit();
	
	NotifyExit(READ_EXIT);

	return NULL;
}


// 轮询函数
void process_poll(struct pollfd* p)
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
bool isReady()
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

// 从fpga获取图像数据
bool ReadImg(VisionNode*& pNode)
{
	if (NULL == ptrData)
	{
		return false;
	}
	
	pNode->lImage = new char[IMG_SIZE + 1];
	if (NULL == pNode->lImage)
	{
		return false;
	}
	
	pNode->rImage = new char[IMG_SIZE + 1];
	if (NULL == pNode->rImage)
	{
		return false;
	}
	
	pNode->lImage[IMG_SIZE] = '\0';
	pNode->rImage[IMG_SIZE] = '\0';
	
	memcpy(pNode->lImage, (char*)ptrData, IMG_SIZE);
	memcpy(pNode->rImage, (char*)ptrData + IMG_SIZE, IMG_SIZE);

	return true;
}

void writeFlg(int fd)
{
	write(fd, "1", 1);
}

// 线程退出函数
void pthread_exit()
{
	pthread_mutex_lock(&qlock);

	int cnt = 0;
	while (pQHead)
	{
		VisionNode* pCur = pQHead;
		pQHead = pQHead->next;
		
		VisionNode::destroy(pCur);
		
		cnt++;
	}
	
	pthread_mutex_unlock(&qlock);
	
	char szData[1024] = {0};
	sprintf(szData, "The num of last vision node is %d\0", cnt);
	
	Writelog(LOG_ERR, szData, __FILE__, __LINE__);
}

// 线程运行标志置为false
void exit_process_vision()
{
	process_running = false;
}

void exit_read_vision()
{
	read_running = false;
}


