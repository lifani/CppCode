/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __108_CAN_CTRL_H__
#define __108_CAN_CTRL_H__

#include "abstractCanCtrl.h"

class C108CanCtrl : public CAbstractCanCtrl
{
public :

	C108CanCtrl();
	
	virtual ~C108CanCtrl();
	
	virtual int Initialize(struct can_frame* pFrame);
};

#endif
