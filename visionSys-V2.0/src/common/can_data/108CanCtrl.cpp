/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "108CanCtrl.h"

C108CanCtrl::C108CanCtrl()
{
}

C108CanCtrl::~C108CanCtrl()
{
	LOGE("C108CanCtrl destroy.");
}

bool C108CanCtrl::CheckHead(char* ptr, int len)
{
	if (CAbstractCanCtrl::CheckHead(ptr, len))
	{
		m_cmdCode = *(unsigned short*)(ptr + 4);
		if (m_cmdCode != 0x1007)
		{
			return true;
		}
	}
	
	return false;
}
