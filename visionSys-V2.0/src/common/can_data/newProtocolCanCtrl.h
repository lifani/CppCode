#ifndef __NEW_PROTOCOL_CAN_CTRL_H__
#define __NEW_PROTOCOL_CAN_CTRL_H__

#include "abstractCanCtrl.h"

class CNewProtocolCanCtrl : public CAbstractCanCtrl
{
public :

	CNewProtocolCanCtrl();
	
	virtual ~CNewProtocolCanCtrl();
	
	virtual int Process(struct can_frame* pFrame);
	
	virtual int GetContent(char*& ptr);
	
	virtual bool CheckHead(char* ptr, int len);
	
	virtual bool CheckTotal(char* ptr, int len);
};

#endif
