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
}

/************************************
���ܣ�	388�������ʼ��
������	pFrame struct can_frame* CAN����֡
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int C388CanCtrl::Initialize(struct can_frame* pFrame)
{
	return CAbstractCanCtrl::Initialize(pFrame);
}

