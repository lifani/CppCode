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
���ܣ�	��������������֣������ء�
������	type ENUM_HF_TYPE ���
		op	int �Զ��������
���أ�	�����ֻ��߱�־
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
���ܣ�	CHF��ʼ�������������̡߳�
������	��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CHF::Initialize()
{
	// ���������߳�
	if (0 != pthread_create(&g_tid, NULL, poll_run, NULL))
	{
		return -1;
	}
	
	return 0;
}

/************************************
���ܣ�	��ȡ����
������	fd int ������
		ptr char* ����ָ�룬����ΪNULL
		len int* ���ݳ��ȣ�����ΪNULL
���أ�	��
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
���ܣ�	��������
������	fd int ������
		ptr char* �������ָ��, ����ΪNULL
		len int ���ݳ���
���أ�	��
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
���ܣ�	ע��
������	��
���أ�	��
************************************/
void CHF::Destroy()
{
	g_running = false;
}

/************************************
���ܣ�	�̹߳���������������
������	arg void* NULL
���أ�	NULL
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
