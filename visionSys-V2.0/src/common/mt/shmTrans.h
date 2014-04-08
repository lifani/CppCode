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

	unsigned short m_fetch;
	unsigned short m_store;
	unsigned short m_total;
	
	SHM_HEAD() : m_fetch(0), m_store(0), m_total
	{
	}
};

#pragma pack()

#define MAX_MSG 10

#define read_lock(fd, offset, len) \
	lock_reg((fd), F_SETLK, F_RDLCK, (offset), SEEK_SET, (len))
#define readw_lock(fd, offset, len) \
	lock_reg((fd), F_SETLKW, F_RDLCK, (offset), SEEK_SET, (len))
#define write_lock(fd, offset, len) \
	lock_reg((fd), F_SETLK, F_WRLCK, (offset), SEEK_SET, (len))
#define writew_lock(fd, offset, len) \
	lock_reg((fd), F_SETLKW, F_WRLCK, (offset), SEEK_SET, (len))
#define un_lock(fd, offset, len) \
	lock_reg((fd), F_SETLK, F_UNLCK, (offset), SEEK_SET, (len))

class CShmCtrl
{
public :

	CShmCtrl(int fd);
	
	~CShmCtrl();
	
	void Init(char*& ptr, off_t offset);
	
	int push(VISION_MSG* pMsg);
	
	int pop(VISION_MSG* pMsg);
	
private :

	int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
	
private :
	
	const int LCK_SIZE;
	int m_fd;
	off_t m_offset;
	
	SHM_HEAD* m_pHead;
	char* m_pData;
};

class CShmTrans : public CTransInterface
{
public :

	static CShmTrans* CreateInstance(const map<long, MSG_TAG*>& mapPMsgTag, key_t key);

	virtual int ReadMsg(VISION_MSG* pMsg);
	
	virtual int WriteMsg(VISION_MSG* pMsg);
	
	virtual ~CShmTrans();
	
protected :
	
	CShmTrans();
	
private :

	int Init(const map<long, MSG_TAG*>& mapPMsgTag, key_t key);

private :

	static CShmTrans* m_pShmTrans;
	
	int 	m_fd;
	int 	m_shmid;
	char* 	m_pShmData;
	char* 	m_pOutData;
	
	map<long, CShmCtrl*> m_mapShmCtrl;
	
	CvSem	m_Sem;
};

#endif
