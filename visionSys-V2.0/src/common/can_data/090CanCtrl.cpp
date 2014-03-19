/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "090CanCtrl.h"

C090CanCtrl::C090CanCtrl()
{
}

C090CanCtrl::~C090CanCtrl()
{
	LOGE("C090CanCtrl destroy.");
}

/************************************
功能：	校验
参数：	无
返回：	正确 true，错误 false
************************************/
bool C090CanCtrl::CheckHead(char* ptr, int len)
{
	if (CAbstractCanCtrl::CheckHead(ptr, len))
	{
		m_cmdCode = *(unsigned short*)(ptr + 4);
		if (m_cmdCode == 0x1002)
		{
			return true;
		}
	}
	
	return false;
}

/************************************
功能：	校验
参数：	无
返回：	正确 true，错误 false
************************************/
bool C090CanCtrl::CheckTotal(char* ptr, int len)
{
	if (CAbstractCanCtrl::CheckTotal(ptr, len))
	{
		return Check4Q();
	}
	
	return false;
}

/************************************
功能：	四元素校验
参数：	无
返回：	正确 0，错误 -1
************************************/
bool C090CanCtrl::Check4Q()
{
	imu_body* p = (imu_body*)m_buf;
	
	if (p->q0 == p->q0 && p->q1 == p->q1 && p->q2 == p->q2 && p->q3 == p->q3)
	{
		float sum = p->q0 * p->q0 + p->q1 * p->q1 + p->q2 * p->q2 + p->q3 * p->q3;
		if (sum > 0.95 && sum < 1.05)
		{
			return true;
		}
	}
	
	return false;
}