/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __090_CAN_CTRL_H__
#define __090_CAN_CTRL_H__

#include "abstractCanCtrl.h"

class C090CanCtrl : public CAbstractCanCtrl
{
public :
	
	C090CanCtrl();
	
	virtual ~C090CanCtrl();

	virtual int Initialize(struct can_frame* pFrame);
	
	virtual bool Check();
	
private :

	bool Check4Q();
};

#endif
