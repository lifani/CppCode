#ifndef __092_CAN_CTRL_H__
#define __092_CAN_CTRL_H__

#include "abstractCanCtrl.h"

class C092CanCtrl : public CAbstractCanCtrl
{
public :

	C092CanCtrl();
	
	virtual ~C092CanCtrl();
	
	virtual int Initialize(struct can_frame* pFrame);
	
	virtual int Process(struct can_frame* pFrame);
	
	virtual bool Check();
};

#endif
