/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __388_CAN_CTRL_H__
#define __388_CAN_CTRL_H__

#include "newProtocolCanCtrl.h"

class C388CanCtrl : public CNewProtocolCanCtrl
{
public :

	C388CanCtrl();
	
	virtual ~C388CanCtrl();
	
	virtual int GetContent(char*& ptr);	
	
	virtual bool CheckHead(char* ptr, int len);

	virtual bool CheckTotal(char* ptr, int len);
};

#endif
