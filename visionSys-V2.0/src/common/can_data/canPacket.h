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
	
	virtual int FD(const char* identify, int op = 0);

	virtual int ReadFd();
	
	virtual int WriteFd();
	
	virtual void GetContent(char* ptr, int* len);
	
	virtual void SetContent(const char* ptr, int len);
	
	virtual HANDLER GetHandler(short event); 
	
private :

	int Initialize(int op);

	int CreateFd(const char* identify);
	
	void SetFilter(struct can_filter* pFilter, unsigned int size);

	void Process388(struct can_frame* pFrame);

	void Process108(struct can_frame* pFrame);
	
	void Process090(struct can_frame* pFrame);
	
	void ChooseHandler(struct can_frame* pFrame);
	
private :
	
	int m_fd;
	int m_key;
	int m_bitrate;
	int m_nContent;
	
	CAN_HEAD m_uCanHead;
	
	CAbstractCanCtrl* m_388CanCtrl;
	CAbstractCanCtrl* m_108CanCtrl;
	CAbstractCanCtrl* m_090CanCtrl;
	
	HANDLER m_rHdl;
	HANDLER m_wHdl;
	
	CQueueCtrl m_388Queue;
	CQueueCtrl m_090Queue;
	
	map<unsigned short, CAbstractCanCtrl*> m_mapRdCanCtrl;
	map<unsigned short, CAbstractCanCtrl*> m_mapWrCanCtrl;
	map<unsigned short, PROC_HANDLER> m_mapProcHandler; 
	
	static map<unsigned short, PROC_HANDLER> g_mapProcHandler;
};

#endif