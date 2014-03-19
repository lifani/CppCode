#ifndef __VISION_STORE_H__
#define __VISION_STORE_H__

#include <platform/base_vision.h>

const unsigned int MEM_CNT = 120;
const unsigned int MEM_SIZE = 76800 * MEM_CNT;

class CVisionStore : public CBaseVision
{
	DECLARE_MESSAGE_MAP
	
public :

	CVisionStore(const char* ppname, const char* pname);
	
	virtual ~CVisionStore();
	
	virtual int ActiveImp();
	
	virtual int DeactiveImp();
	
	void ProcessMsg(VISION_MSG* pMsg);
	
	void Store();
	
private :

	int InitOption();
	
	int InitStore();
	
	int InitMem();
	
	int ResetStore();
	
	int CreateDir(const char* ptr);
	
	int RemoveDir(const char* ptr);
	
	int ExecCmd(const char* ptr);
	
	int OutFile(const char* path, const char* ptr, int len);
	
private :

	string m_strPath;
	
	int m_num;
	int m_dIndex;
	int m_fIndex;
	int m_files;
	int m_folders;
	
	unsigned int m_above;
	bool m_ready;
	
	char* m_ptr;
	char* m_ptrIndex[MEM_CNT];
	
	unsigned int m_fetchPos;
	unsigned int m_storePos;
	
	pthread_mutex_t m_lock;
	pthread_cond_t m_cond;
};

#endif
