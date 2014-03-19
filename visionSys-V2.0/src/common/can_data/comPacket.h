#ifndef __COM_PACKET_H__
#define __COM_PACKET_H__

#include "abstractPacket.h"
#include "queueCtrl.h"

#ifndef LOG_TAG
#define LOG_TAG "CHF"
#endif

typedef struct tUART_DATA
{
	unsigned char head;
	unsigned char data[22];
	unsigned char tail[2];
} UART_DATA;

const int MAX_SIZE = 2 * sizeof(UART_DATA);

class CComPacket : public CAbstractPacket
{
public :

	CComPacket();
	
	virtual ~CComPacket();
	
	int FD(const char* identify = NULL, int op = 0);
	
	int ReadFd();
	
	int WriteFd();
	
	void GetContent(char* ptr, int* len);
	
	void SetContent(const char* ptr, int len);
	
	HANDLER GetHandler(short event);
	
private :

	int CreateFd();
	
	int process(char* ptr, int len);
	
	bool check(char* ptr);
	
private :

	int m_fd;
	
	HANDLER m_rHdl;
	HANDLER m_wHdl;
	
	char m_szBuf[MAX_SIZE];
	
	int m_storePos;
	int m_fetchPos;
	
	int m_baudrate;
	char m_databits;
	char m_paritybits;
	char m_stopbits;
	
	CQueueCtrl m_uartCtrl;
};

#endif
