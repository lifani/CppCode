#include "newProtocolCanCtrl.h"

#include "MF_CRC8.h"
#include "crc16.h"

CNewProtocolCanCtrl::CNewProtocolCanCtrl()
{
}

CNewProtocolCanCtrl::~CNewProtocolCanCtrl()
{
	LOGE("CNewProtocolCanCtrl destroy.");
}

int CNewProtocolCanCtrl::Process(struct can_frame* pFrame)
{
	if (NULL == pFrame)
	{
		return -1;
	}
	
	int err = -1;
	if (CheckHead((char*)pFrame->data, pFrame->can_dlc))
	{
		unsigned short size = *(unsigned short*)((char*)pFrame->data + 1);
		if (size > sizeof(m_buf))
		{
			return err;
		}

		m_size = (unsigned int)size;
		m_pos = 0;
		m_begin = true;
	}
	
	if (m_begin)
	{
		for (int i = 0; i < pFrame->can_dlc && m_pos < m_size; ++i, ++m_pos)
		{
			m_buf[m_pos] = pFrame->data[i];
		}
		
		if (m_pos == m_size)
		{
			err = CheckTotal(m_buf, m_size) ? 0 : -1;
			m_begin = false;
		}
	}
	
	return err;
}

int CNewProtocolCanCtrl::GetContent(char*& ptr)
{
	for (unsigned int i = 6; i < m_size - 1; ++i)
	{
		m_buf[i] ^= m_key;
	}
	
	ptr = m_buf + 6;
	
	return (int)m_size - 6 - 1;
}

bool CNewProtocolCanCtrl::CheckHead(char* ptr, int len)
{
	if (NULL == ptr || len < 4)
	{
		return false;
	}
	
	if (0x55 == *ptr && Verify_CRC8_Check_Sum((unsigned char*)ptr, 4))
	{
		return true;
	}

	return false;
}

bool CNewProtocolCanCtrl::CheckTotal(char* ptr, int len)
{
	if (NULL == ptr)
	{
		return false;
	}
		
	return Verify_CRC16_Check_Sum((unsigned char*)ptr, len);
}