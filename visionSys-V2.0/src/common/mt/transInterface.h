#ifndef __TRANS_INTERFACE_H__
#define __TRANS_INTERFACE_H__

class CTransInterface
{
public :
	
	virtual ~CTransInterface() {};
	
	virtual int ReadMsg(VISION_MSG* pMsg) = 0;
	
	virtual int WriteMsg(VISION_MSG* pMsg) = 0;
};

#endif
