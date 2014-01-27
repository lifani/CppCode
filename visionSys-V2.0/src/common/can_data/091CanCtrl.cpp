/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "091CanCtrl.h"

C091CanCtrl::C091CanCtrl() : m_pos(0), m_size(0)
{
}

C091CanCtrl::~C091CanCtrl()
{
}

/************************************
功能：	091处理类初始化
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 0，失败 -1
************************************/
int C091CanCtrl::Initialize(struct can_frame* pFrame)
{
	m_tCanHead.head[0] = 0x55;
	m_tCanHead.head[1] = 0xaa;
	m_tCanHead.head[2] = 0x55;
	m_tCanHead.head[3] = 0xaa;
	
	m_tCanHead.cmd_code = 0x1005;
	
	m_tCanTail.tail[0] = 0x66;
	m_tCanTail.tail[1] = 0xcc;
	m_tCanTail.tail[2] = 0x66;
	m_tCanTail.tail[3] = 0xcc;
	return 0;
}

/************************************
功能：	数据帧处理
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 >= 0，失败 -1
************************************/
int C091CanCtrl::Process(struct can_frame* pFrame)
{
	if (NULL == pFrame || 0 == m_size)
	{
		return -1;
	}
	
	if (m_pos == m_size)
	{
		m_pos = 0;
		m_size = 0;
		
		return 0;
	}
	
	pFrame->can_id = 0x091;

	unsigned int i = 0;
	for (; i < FRAME_LEN && m_pos < m_size; ++i, ++m_pos)
	{
		pFrame->data[i] = m_szData[m_pos];
	}
	
	pFrame->can_dlc = i;
	
	return pFrame->can_dlc;
}

/************************************
功能：	设置发送内容
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 0，失败 -1
************************************/
int C091CanCtrl::SetContent(char* ptr, int len)
{
	if (NULL == ptr || 0 == len)
	{
		return -1;
	}
	
	m_tCanHead.len = len;
	
	if (0 == m_size)
	{
		memcpy(m_szData + m_size, (char*)&m_tCanHead, sizeof(CAN_HEAD_T));
		m_size += sizeof(CAN_HEAD_T);
		
		memcpy(m_szData + m_size, ptr, len);
		m_size += len;
		
		memcpy(m_szData + m_size, (char*)&m_tCanTail, sizeof(CAN_TAIL_T));
		m_size += sizeof(CAN_TAIL_T);
		
		m_szData[m_size] = '\0';
	}
	
	return 0;
}