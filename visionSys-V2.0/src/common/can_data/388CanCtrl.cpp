/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "388CanCtrl.h"

C388CanCtrl::C388CanCtrl()
{
}

C388CanCtrl::~C388CanCtrl()
{
	LOGE("C388CanCtrl destroy.");
}

int C388CanCtrl::GetContent(char*& ptr)
{
	ptr = m_buf + 4;
	
	// ���ṹ: 0x55 + ����(�����ֽ�) + У���(1���ֽ�) + ���� + У���(�����ֽ�)
	return (int)m_size - 4 - 2;
}

bool C388CanCtrl::CheckHead(char* ptr, int len)
{
	if (CNewProtocolCanCtrl::CheckHead(ptr, len))
	{
		if (0xA2 == *(ptr + 7) || 0xC2 == *(ptr + 7))
		{
			return true;
		}
	}
	
	return false;
}

bool C388CanCtrl::CheckTotal(char* ptr, int len)
{
	if (CNewProtocolCanCtrl::CheckTotal(ptr, len))
	{
		if (0xA2 == *(ptr + 7) || 0xC2 == *(ptr + 7))
		{
			return true;
		}
	}
	
	return false;
}