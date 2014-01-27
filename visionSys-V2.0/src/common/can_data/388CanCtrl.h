/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __388_CAN_CTRL_H__
#define __388_CAN_CTRL_H__

#include "abstractCanCtrl.h"

class C388CanCtrl : public CAbstractCanCtrl
{
public :

	C388CanCtrl();
	
	virtual ~C388CanCtrl();
	
	virtual int Initialize(struct can_frame* pFrame);
	
};

#endif
