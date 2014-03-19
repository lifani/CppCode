#include "comPacket.h"

#include <sbus.h>

CComPacket::CComPacket()
: m_fd(0)
, m_rHdl((HANDLER)0)
, m_wHdl((HANDLER)0)
, m_storePos(0)
, m_fetchPos(0)
, m_baudrate(100000)
, m_databits(8)
, m_paritybits('E')
, m_stopbits(2)
, m_uartCtrl(sizeof(UART_DATA), 10, true)
{
}

CComPacket::~CComPacket()
{
	LOGE("CComPacket destroy.");
}

int CComPacket::FD(const char* identify, int op)
{
	if (-1 == CreateFd())
	{
		return -1;
	}
	
	m_rHdl = static_cast<HANDLER>(&CComPacket::ReadFd);
	
	if (-1 == m_uartCtrl.Initialize())
	{
		return -1;
	}
	
	return m_fd;
}

int CComPacket::CreateFd()
{
	char *arm_path[] = {"/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2"};
	
	m_fd = open(arm_path[UART_PORT2 - 1], O_RDWR | O_NOCTTY);
	if (m_fd < 0)
	{
		LOGE("open uart err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (uart_config(m_fd, m_baudrate, m_databits, m_paritybits, m_stopbits) < 0)
	{
		LOGE("uart config err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

int CComPacket::ReadFd()
{
	char szBuf[25] = {0};
	int len = read(m_fd, szBuf, 25);
	if (len > 0)
	{
		process(szBuf, len);
	}
	
	return len;
}

int CComPacket::WriteFd()
{
	return 0;
}

void CComPacket::GetContent(char* ptr, int* len)
{
	if (NULL == ptr || NULL == len)
	{
		return;
	}
	
	*len = sizeof(UART_DATA);
	m_uartCtrl.pop(ptr);
	
	return;
}

void CComPacket::SetContent(const char* ptr, int len)
{
	return;
}

HANDLER CComPacket::GetHandler(short event)
{	
	HANDLER hdl = NULL;
	
	if (event & POLLIN)
	{
		hdl = m_rHdl;
	}
	
	return hdl;
}

int CComPacket::process(char* ptr, int len)
{
	if (NULL == ptr || 0 == len || (unsigned int)len > sizeof(UART_DATA))
	{
		return -1;
	}
	
	if (check(ptr))
	{
		m_uartCtrl.push(ptr);
	}
	
	return 0;
}

bool CComPacket::check(char* ptr)
{
	if (NULL == ptr)
	{
		return false;
	}
	
	UART_DATA* p = (UART_DATA*)ptr;

	
	if (0x0F == p->head && 0x04 == (p->tail[1] & 0x0F))
	{
		return true;
	}
	
	return false;
}

