/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __CAN_PACKET_H__
#define __CAN_PACKET_H__

#include "abstractPacket.h"
#include "abstractCanCtrl.h"
#include "queueCtrl.h"

#ifndef LOG_TAG
#define LOG_TAG "CHF"
#endif

class CCanPacket : public CAbstractPacket
{
public :
	
	CCanPacket();
	
	virtual ~CCanPacket();
	
	virtual int FD(const char* identify = NULL, int op = 0);

	virtual int ReadFd();
	
	virtual int WriteFd();
	
	virtual void GetContent(char* ptr, int* len);
	
	virtual void SetContent(const char* ptr, int len);
	
	virtual HANDLER GetHandler(short event); 
	
private :

	int Initialize(int op);

	int CreateFd(const char* identify);
	
	void SetFilter(struct can_filter* pFilter, unsigned int size);
	
	void Process(struct can_frame* pFrame);

	void Process388(char* ptr, int len);

	void Process109(char* ptr, int len);
	
	void Process108(char* ptr, int len);
	
	void Process092(char* ptr, int len);
	
	void Process090(char* ptr, int len);
	
private :
	
	map<unsigned short, CAbstractCanCtrl*> m_mapRdCanCtrl;
	map<unsigned short, CAbstractCanCtrl*> m_mapWrCanCtrl;
	map<unsigned short, PROC_HANDLER> m_mapProcHandler; 
	
	int m_fd;
	int m_key;
	int m_bitrate;
	int m_nContent;
	
	unsigned int m_index;
	
	CAN_HEAD m_uCanHead;
	
	HANDLER m_rHdl;
	HANDLER m_wHdl;
	
	pthread_mutex_t m_lock;
	
	CQueueCtrl m_388Queue;
	CQueueCtrl m_imuQueue;
	
	char m_tBuf[256];
};

#endif