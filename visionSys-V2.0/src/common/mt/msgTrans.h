#ifndef __MSG_TRANS_H__
#define __MSG_TRANS_H__

#include <typedef.h>
#include <datatype.h>
#include "transInterface.h"

class CMsgTrans : public CTransInterface
{
public :

	static CMsgTrans* CreateInstance(key_t key);
	
	virtual int ReadMsg(VISION_MSG* pMsg);
	
	virtual int WriteMsg(VISION_MSG* pMsg);
	
	virtual ~CMsgTrans();
	
protected :

	CMsgTrans();
	
private :

	int Init(key_t key);
	
private :

	static CMsgTrans* m_pMsgTrans;
	
	int m_msgqid;
};

#endif 
