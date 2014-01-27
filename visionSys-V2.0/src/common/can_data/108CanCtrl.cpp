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
}

/************************************
功能：	108处理类初始化
参数：	pFrame struct can_frame* CAN数据帧
返回：	成功 0，失败 -1
************************************/
int C108CanCtrl::Initialize(struct can_frame* pFrame)
{
	if (FRAME_LEN != pFrame->can_dlc)
	{
		return -1;
	}

	m_cmdCode = *(unsigned short*)(pFrame->data + 4);
	if (m_cmdCode == 0x1007)
	{
		return -1;
	}
	return CAbstractCanCtrl::Initialize(pFrame);
}