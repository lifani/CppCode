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
功能：	388处理类初始化
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 0，失败 -1
************************************/
int C388CanCtrl::Initialize(struct can_frame* pFrame)
{
	return CAbstractCanCtrl::Initialize(pFrame);
}

