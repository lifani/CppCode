/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "queueCtrl.h"


CCanQueueCtrl::CCanQueueCtrl(unsigned int t_size, unsigned int t_cnt, unsigned mode)
: m_fetchPos(0)
, m_storePos(0)
, m_size(t_size)
, m_cnt(0)
, m_maxCnt(t_cnt)
, m_totalSize(t_size * t_cnt)
, m_mode(mode)
, m_index(0)
, m_ptr(0)
{
	pthread_mutex_init(&m_lock, NULL);
}

CCanQueueCtrl::~CCanQueueCtrl()
{
	if (NULL != m_ptr)
	{
		delete []m_ptr;
		m_ptr = NULL;
	}
	
	pthread_mutex_destroy(&m_lock);
}

/************************************
功能：	队列初始化
参数：	无
返回：	成功 0，失败 -1
************************************/
int CCanQueueCtrl::Initialize()
{
	if (NULL == m_ptr)
	{
		m_ptr = new char[m_totalSize + 1];
		if (NULL == m_ptr)
		{
			return -1;
		}
		
		memset(m_ptr, 0, m_totalSize + 1);
	}
	
	return 0;
}

/************************************
功能：	入队操作
参数：	ptr char* 内容指针, 不能为NULL
返回：	成功 0，失败 -1
************************************/
int CCanQueueCtrl::push(char* ptr)
{
	if (NULL == ptr)
	{
		return -1;
	}
	
	if (m_cnt == m_maxCnt)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_lock);
	
	memcpy(m_ptr + m_storePos, ptr, m_size);

	if (m_index++ > m_mode)
	{
		m_fetchPos += m_size;
		if (m_totalSize == m_fetchPos)
		{
			m_fetchPos = 0;
		}
	}
	
	m_storePos += m_size;
	if (m_totalSize == m_storePos)
	{
		m_storePos = 0;
	}
	
	++m_cnt;
	
	pthread_mutex_unlock(&m_lock);
	
	return 0;
}

/************************************
功能：	出队操作
参数：	ptr char* 内容指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CCanQueueCtrl::pop(char* ptr)
{	
	if (NULL == ptr)
	{
		return -1;
	}
	
	if (0 == m_cnt)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_lock);
	
	memcpy(ptr, m_ptr + m_fetchPos, m_size);
	
	/*if (!m_mode)
	{
		m_fetchPos += m_size;
		if (m_totalSize == m_fetchPos)
		{
			m_fetchPos = 0;
		}
	}*/
	
	--m_cnt;
	
	pthread_mutex_unlock(&m_lock);

	return 0;
}

