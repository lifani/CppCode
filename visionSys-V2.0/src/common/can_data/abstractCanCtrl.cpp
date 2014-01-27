/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "abstractCanCtrl.h"

CAbstractCanCtrl::CAbstractCanCtrl()
: m_cmdCode(0), m_size(0), m_pos(0), m_buf(0), m_key(0)
{
}

CAbstractCanCtrl::~CAbstractCanCtrl()
{
	if (NULL != m_buf)
	{
		delete []m_buf;
		m_buf = NULL;
	}
}

/************************************
功能：	CAN基类初始化，可覆盖
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 0，失败 -1
************************************/
int CAbstractCanCtrl::Initialize(struct can_frame* pFrame)
{
	if (FRAME_LEN != pFrame->can_dlc)
	{
		return -1;
	}
	
	unsigned short size = *(unsigned short*)(pFrame->data + 6);
	
	if (size > m_size)
	{
		if (NULL != m_buf)
		{
			delete []m_buf;
			m_buf = NULL;
		}
		
		m_buf = new char[size + 4 + 1];
	}
	
	m_size = size;
	if (NULL == m_buf)
	{
		return -1;
	}
	
	m_buf[m_size + 4] = '\0';
	
	m_uCanHead.szHead[0] = 0x55;
	m_uCanHead.szHead[1] = 0xaa;
	m_uCanHead.szHead[2] = 0x55;
	m_uCanHead.szHead[3] = 0xaa;
	
	m_uCanTail.szTail[0] = 0x66;
	m_uCanTail.szTail[1] = 0xcc;
	m_uCanTail.szTail[2] = 0x66;
	m_uCanTail.szTail[3] = 0xcc;
	
	return 0;
}

/************************************
功能：	数据帧处理函数
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 > 0, 失败 -1
************************************/
int CAbstractCanCtrl::Process(struct can_frame* pFrame)
{
	for (unsigned int i = 0; i < pFrame->can_dlc && m_pos < m_size + 4; ++i, ++m_pos)
	{
		if (m_pos < m_size)
		{
			m_buf[m_pos] = *(pFrame->data + i) ^ m_key;
		}
		else
		{
			m_buf[m_pos] = *(pFrame->data + i);
		}
	}
	
	int err = 0;
	if (m_pos == m_size + 4)
	{
		err = Check() ? m_size : -1;
		m_pos = 0;
	}
	
	return err;
}

/************************************
功能：	获取内容
参数：	ptr char*& 内容指针引用
返回：	内容长度
************************************/
int CAbstractCanCtrl::GetContent(char*& ptr)
{
	ptr = m_buf;
	
	return m_size;
}

/************************************
功能：	设置内容
参数：	ptr char* 内容指针
		len int 内容长度
返回：	成功 0， 失败 -1
************************************/
int CAbstractCanCtrl::SetContent(char* ptr, int len)
{
	return 0;
}

/************************************
功能：	CAN数据包校验函数
参数：	无
返回：	成功 true, 失败 false
************************************/
bool CAbstractCanCtrl::Check()
{
	unsigned int uTail = *(unsigned int*)(m_buf + m_size);
	
	return m_uCanTail.uTail == uTail;
}

/************************************
功能：	CAN基类初始化
参数：	pFrame struct can_frame* CAN数据帧
返回：	无
************************************/
void CAbstractCanCtrl::SetKey(int key)
{
	m_key = key;
}
