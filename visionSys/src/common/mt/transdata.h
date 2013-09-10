#ifndef __TRANS_DATA__
#define __TRANS_DATA__

class CTransData
{
public :

	virtual ~CTransData() {};

	virtual int Init() = 0;
	
	virtual int write(const char* ptr, unsigned int size) = 0;
	
	virtual int read(char* ptr, unsigned int size) = 0;
	
	virtual void destory(CTransData* p) = 0;
};

#endif
