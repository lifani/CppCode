/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __QUEUE_CTRL_H__
#define __QUEUE_CTRL_H__

#include <typedef.h>

class CQueueCtrl
{
public :
	
	// mode 0: 先进先出 1：后进先出 
	CQueueCtrl(unsigned int t_size, unsigned int t_cnt, bool mode = false);
	
	~CQueueCtrl();
	
	int Initialize();
	
	int push(char* ptr);
	
	int pop(char* ptr);
	
private :

	unsigned int m_fetchPos;
	unsigned int m_storePos;
	unsigned int m_size;
	unsigned int m_cnt;
	const unsigned int m_maxCnt;
	const unsigned int m_totalSize;
	
	bool m_mode;
	char* m_ptr;
	
	pthread_mutex_t m_lock;
};

#endif