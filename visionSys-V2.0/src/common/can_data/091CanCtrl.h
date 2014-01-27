/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __091_CAN_CTRL_H__
#define __091_CAN_CTRL_H__

#include "abstractCanCtrl.h"

typedef struct _CAN_HEAD_T
{
	char head[4];
	unsigned short cmd_code;
	unsigned short len;
} CAN_HEAD_T;

typedef struct _CAN_TAIL_T
{
	char tail[4];
} CAN_TAIL_T;


class C091CanCtrl : public CAbstractCanCtrl
{
public :

	C091CanCtrl();
	
	virtual ~C091CanCtrl();

	virtual int Initialize(struct can_frame* pFrame);
	
	virtual int Process(struct can_frame* pFrame);
	
	virtual int SetContent(char* ptr, int len);
	
private :

	CAN_HEAD_T m_tCanHead;
	CAN_TAIL_T m_tCanTail;
	
	unsigned int m_pos;
	unsigned int m_size;
	
	char m_szData[256];
};

#endif
