#ifndef __COMPOENT_INTERFACE__
#define __COMPOENT_INTERFACE__

#include <typedef.h>
#include <datatype.h>

class CCompoentInterface
{
public:

	virtual ~CCompoentInterface() {}
	
	virtual int Active() = 0;
	
	virtual int Action() = 0;
	
	virtual int Deactive() = 0;
	
	virtual void AddConfig(vector<MSG_CONFIG>& vMsgXmlNode, vector<PROC_CONFIG>& vProcXmlNode) = 0;
	
	virtual void AddProcInfo(const char* pname, int pid) = 0;
	
	virtual void AddOption(vector<OPTION>& vOption) = 0;
};

extern "C" CCompoentInterface* CreateInstance(const char* ppname, const char* pname);

#endif
