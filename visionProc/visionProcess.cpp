#include "visionProcess.h"
#include "../odometer/interface.h"

VisionNode* pQHead = NULL;
VisionNode* pQTail = NULL;

void* ptrData = NULL;
int fd = 0;

pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;

// 视觉处理线程入口函数
void* process_vision(void* arg)
{
    VisionNode* pNode = NULL;
    for (;;)
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
        Run(pNode->lImage, pNode->rImage, pNode->imu);

        // 处理结束后释放空间
        if (NULL != pNode)
        {
            pNode->destroy(pNode);
            pNode = NULL;
        }

        pthread_mutex_unlock(&qlock);
    }

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

    VisionNode* pNode = NULL;

    // 轮询、获取图片数据

    int st_fd = open(DEVICE_SYS_POLL, O_RDWR);
    if (st_fd < 0)
    {
        Writelog(LOG_ERR, "Open poll file failed.");
        return NULL;
    }

    struct pollfd p_fd;

    p_fd.fd = st_fd;
    p_fd.events = POLLERR | POLLHUP;


    for (;;)
    {
        pNode = VisionNode::Instance();

        // 调用轮询函数
	process_poll(&p_fd);

        // 读取数据
        //if (!Read(pNode->lImage, pNode->rImage, pNode->imu))
        //{
            //break;
        //}
		
	if (!ReadImg(pNode))
	{
		writeFlg(p_fd.fd);
		VisionNode::destroy(pNode);
			
		continue;
	}
		
	writeFlg(p_fd.fd);

        // 入队列
        enter_vision_queue(pNode);
        pNode = NULL;
    }

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

        if (err && isReady(p->fd))
        {
            break;
        }
    }
}

bool isReady(int fd)
{
    unsigned char install = '\0';
    int err = read(fd, &install, 1);
    if (err == 1 && install == '1')
    {
        return true;
    }

    return false;
}

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
	
	memcpy(pNode->lImage, ptrData, IMG_SIZE);
	memcpy(pNode->rImage, (char*)ptrData + IMG_SIZE, IMG_SIZE);
	
	return true;
}

void writeFlg(int fd)
{
    write(fd, "1", 1);
}

void Initlog(const char* strProcName)
{
    openlog(strProcName, LOG_ODELAY | LOG_PID, LOG_USER);
}

void Writelog(int priority, const char* strErrInfo, const char* strFileName, int line )
{
    syslog(priority, "log : %s at file: %s line: %d", strErrInfo, strFileName, line);
}

void daemonize(void)
{
    int                 fd0, fd1, fd2;
    pid_t               pid;
    struct rlimit       rl;
    struct sigaction    sa;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        Writelog(LOG_ERR, "can't get file limit.", __FILE__, __LINE__);
        return;
    }

    if ((pid = fork()) < 0)
    {
        Writelog(LOG_ERR, "can't fork.", __FILE__, __LINE__);
        return;
    }
    else if (pid != 0)
    {
        exit(0);
    }

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        Writelog(LOG_ERR, "can't ignore SIGHUP", __FILE__, __LINE__);
        return;
    }

    if ((pid = fork()) < 0)
    {
        Writelog(LOG_ERR, "can't fork", __FILE__, __LINE__);
        return;
    }
    else if (pid != 0)
    {
        exit(0);
    }

    if (chdir("./") < 0)
    {
        Writelog(LOG_ERR, "can't change directory to ./", __FILE__, __LINE__);
        return;
    }

    if (rl.rlim_max == RLIM_INFINITY)
    {
        rl.rlim_max = 1024;
    }

    for (unsigned int i = 0; i < rl.rlim_max; i++)
    {
        close(i);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        Writelog(LOG_ERR, "unexpected file descriptors", __FILE__, __LINE__);
        return;
    }

    return;
}

