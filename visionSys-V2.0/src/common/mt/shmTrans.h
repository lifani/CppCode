/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __SHM_TRANS_H__
#define __SHM_TRANS_H__

#include <typedef.h>
#include <datatype.h>
#include "transInterface.h"
#include "vSem.h"

#pragma pack(1)
class SHM_HEAD
{
public :
	unsigned int m_initialized;
	unsigned int m_size;
	unsigned short m_cnt;
	unsigned short m_fetch;
	unsigned short m_store;
	unsigned short res;
	
	SHM_HEAD() : m_initialized(0), m_size(0), m_cnt(0), m_fetch(0), m_store(0), res(0)
	{
	}
	
};

#pragma pack()

class CShmCtrl
{
public :

	CShmCtrl(MSG_CONFIG& tMsgConfig);
	
	~CShmCtrl();
	
	void Init(char*& ptr);
	
	int push(VISION_MSG* pMsg);
	
	int pop(VISION_MSG* pMsg);
	
private :
	MSG_CONFIG m_tMsgConfig;
	
	unsigned int m_uCnt;

	SHM_HEAD * m_pHead;
	char* m_pData;
	
	long m_id;
	
	static char* m_lastPtr;
};

class CShmTrans : public CTransInterface
{
public :

	static CShmTrans* CreateInstance(vector<MSG_CONFIG>& vMsgConfig, key_t key);

	virtual int ReadMsg(VISION_MSG* pMsg);
	
	virtual int WriteMsg(VISION_MSG* pMsg);
	
	virtual ~CShmTrans();
	
protected :
	
	CShmTrans();
	
private :

	int Init(vector<MSG_CONFIG>& vMsgConfig, key_t key);
	
	int ReadDeamonMsg(VISION_MSG* pMsg);
	
	int WriteDeamonMsg(VISION_MSG* pMsg);

private :

	static CShmTrans* m_pShmTrans;
	
	int 	m_shmid;
	char* 	m_pShmData;
	char* 	m_pOutData;
	
	map<long, CShmCtrl*> m_mapShmCtrl;
	
	CvSem	m_Sem;
};

#endif
