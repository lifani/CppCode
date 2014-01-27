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
}

/************************************
���ܣ�	090�������ʼ��
������	pFrame struct can_frame* CAN����֡
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int C090CanCtrl::Initialize(struct can_frame* pFrame)
{
	if (FRAME_LEN != pFrame->can_dlc)
	{
		return -1;
	}
	
	m_cmdCode = *(unsigned short*)(pFrame->data + 4);
	if (m_cmdCode == 0x1001)
	{
		return -1;
	}

	return CAbstractCanCtrl::Initialize(pFrame);
}

/************************************
���ܣ�	У��
������	��
���أ�	��ȷ true������ false
************************************/
bool C090CanCtrl::Check()
{
	if (CAbstractCanCtrl::Check())
	{
		return Check4Q();
	}
	
	return false;
}

/************************************
���ܣ�	��Ԫ��У��
������	��
���أ�	��ȷ 0������ -1
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