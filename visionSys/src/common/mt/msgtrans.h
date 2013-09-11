#ifndef __MSG_TRANS_H__
#define __MSG_TRANS_H__

#include <sys/msg.h>
#include "transdata.h"

class CMsgTrans : public CTransData
{
public :
	
	CMsgTrans(key_t key);
	
	virtual int Init();
	
	virtual int write( char* ptr, unsigned int* size);
	
	virtual int read(char* ptr, unsigned int* size);
	
	virtual void destory(CTransData* p);
private :

	virtual ~CMsgTrans();
	
private :

	key_t m_key;
	int m_msgid;
	
	int m_initialized;
};

#endif