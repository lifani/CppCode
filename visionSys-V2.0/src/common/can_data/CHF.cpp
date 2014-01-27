/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
// 1st. FD(ENUM_HF_TYPE)
// 2nd. Initialize()
// 3th. GetContent or SetContent

#include "CHF.h"
#include "canPacket.h"

static unsigned int g_pos = 0;
static bool g_running = true;
static unsigned int g_uTaskNum = 0;
static pthread_t g_tid = 0;

CTask::CTask() : m_pPacket(NULL), m_handler(NULL)
{
}

CTask::~CTask()
{
}

map<int, CAbstractPacket*> CHF::g_mapFdPacket;
CTask CHF::g_ArrayTask[10];
struct pollfd CHF::g_ArrayPollFd[5];

/************************************
功能：	按类别生成描述字，并返回。
参数：	type ENUM_HF_TYPE 类别
		op	int 自定义操作数
返回：	描述字或者标志
************************************/
int CHF::FD(ENUM_HF_TYPE type, int op)
{
	if (g_pos >= 5)
	{
		return -1;
	}
	
	int fd = 0;
	CAbstractPacket* pPacket = NULL;
	
	switch(type)
	{
	case HF_CAN0:
	{
		pPacket = new CCanPacket;
		if (NULL == pPacket)
		{
			return -1;
		}
		
		fd = pPacket->FD("can0", op);
		
		g_ArrayPollFd[g_pos].events = POLLIN | POLLOUT;
		
		break;
	}
	default:
		break;
	};
	
	if (fd == 0)
	{
		delete pPacket;
		pPacket = NULL;
		
		return -1;
	}
	
	g_mapFdPacket[fd] = pPacket;
	g_ArrayPollFd[g_pos].fd = fd;
	
	++g_pos;
	
	return fd;
}

/************************************
功能：	CHF初始化，启动工作线程。
参数：	无
返回：	成功 0，失败 -1
************************************/
int CHF::Initialize()
{
	// 启动接收线程
	if (0 != pthread_create(&g_tid, NULL, poll_run, NULL))
	{
		return -1;
	}
	
	return 0;
}

/************************************
功能：	获取内容
参数：	fd int 描述字
		ptr char* 内容指针，不能为NULL
		len int* 内容长度，不能为NULL
返回：	无
************************************/
void CHF::GetContent(int fd, char* ptr, int* len)
{
	if (NULL == ptr || NULL == len)
	{
		return;
	}
	
	CAbstractPacket* pPacket = NULL;
	if (NULL != (pPacket = g_mapFdPacket[fd]))
	{
		pPacket->GetContent(ptr, len);
	}
}

/************************************
功能：	设置内容
参数：	fd int 描述字
		ptr char* 输出内容指针, 不能为NULL
		len int 内容长度
返回：	无
************************************/
void CHF::SetContent(int fd, const char* ptr, int len)
{
	if (NULL == ptr || 0 == len)
	{
		return;
	}
	
	CAbstractPacket* pPacket = NULL;
	if (NULL != (pPacket = g_mapFdPacket[fd]))
	{
		pPacket->SetContent(ptr, len);
	}
}

/************************************
功能：	注销
参数：	无
返回：	无
************************************/
void CHF::Destroy()
{
	g_running = false;
}

/************************************
功能：	线程工作函数，工作泵
参数：	arg void* NULL
返回：	NULL
************************************/
void* CHF::poll_run(void* arg)
{
	while (g_running)
	{
		for (unsigned int i = 0; i < g_uTaskNum; ++i)
		{
			if (NULL != g_ArrayTask[i].m_pPacket && HANDLER(0) != g_ArrayTask[i].m_handler)
			{
				g_ArrayTask[i].DoHandle();
				
				g_ArrayTask[i].m_pPacket = NULL;
				g_ArrayTask[i].m_handler = NULL;
			}
		}
		
		int err = poll(g_ArrayPollFd, g_pos, 2);
		if (err > 0)
		{
			g_uTaskNum = 0;
			for (unsigned int i = 0; i < g_pos; ++i)
			{
				if (g_ArrayPollFd[i].revents & (POLLIN | POLLOUT))
				{
					int fd = g_ArrayPollFd[i].fd;
					
					if (g_ArrayPollFd[i].revents & POLLIN)
					{
						g_ArrayTask[g_uTaskNum].m_pPacket = g_mapFdPacket[fd];
						g_ArrayTask[g_uTaskNum].m_handler = g_mapFdPacket[fd]->GetHandler(POLLIN);
						
						++g_uTaskNum;
					}
					
					if (g_ArrayPollFd[i].revents & POLLOUT)
					{
						g_ArrayTask[g_uTaskNum].m_pPacket = g_mapFdPacket[fd];
						g_ArrayTask[g_uTaskNum].m_handler = g_mapFdPacket[fd]->GetHandler(POLLOUT);
						
						++g_uTaskNum;
					}
				}
				else
				{
					g_ArrayTask[i].m_pPacket = NULL;
					g_ArrayTask[i].m_handler = NULL;
				}
			}
		}
	}
	
	return NULL;
}
