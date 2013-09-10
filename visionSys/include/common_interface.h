#ifndef __COMMON_INTERFACE__
#define __COMMON_INTERFACE__

class CCommonInterface
{
public:

	virtual ~CCommonInterface() {}
	
	virtual int Active() = 0;
	
	virtual int Action() = 0;
	
	virtual int Deactive() = 0;
	
	virtual void AddSonProcInfo(const char* pname, int pid) = 0;
};

extern "C" CCommonInterface* CreateInstance(const char* ppname, const char* pname);

#endif
