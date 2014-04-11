/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __CHF_H__
#define __CHF_H__

#include <typedef.h>
#include <datatype.h>

#include "abstractPacket.h"

enum ENUM_HF_TYPE
{
	HF_CAN0,
	HF_CAN1,
	HF_COM,
	HF_SOCKET,
	HF_NORMAL
};

class CTask
{
public :

	CTask();
	
	~CTask();

	int DoHandle()
	{
		if (0 == m_pPacket || HANDLER(0) == m_handler)
		{
			return -1;
		}
		
		return (m_pPacket->*m_handler)();
	}

public :
	CAbstractPacket* m_pPacket;
	
	HANDLER m_handler;
};

class CHF
{
public :

	static int FD(ENUM_HF_TYPE type, int op = 0);
	
	static int Initialize(bool flg = true);
	
	static void GetContent(int fd, char* ptr, int* len);
	
	static void SetContent(int fd, const char* ptr, int len);
	
	static void Destroy();
	
	static void* poll_run(void* arg);
	
private :

	static map<int, CAbstractPacket*> g_mapFdPacket;
	static CTask g_ArrayTask[10];
	static struct pollfd g_ArrayPollFd[5];
	
	static bool m_flg;
};

#endif