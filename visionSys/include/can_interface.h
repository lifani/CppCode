#ifndef __CAN_INTERFACE__
#define __CAN_INTERFACE__

#include <vector>
using namespace std;

typedef struct _filter_param
{
	unsigned short m_usCanId;
	unsigned short m_usCmdCode;
} Filter_param;

class Packet;

class can_interface
{
public :
	
	virtual ~can_interface()
	{
	}
	
	virtual Packet* Read() = 0;
	
	virtual int Write(const char* ptr, unsigned int len, unsigned short can_id = 0, unsigned short cmd_code = 0) = 0;

	virtual bool Init() = 0;
	
	virtual bool SetFilter(vector<Filter_param> &rFilterParam) = 0;
	
	virtual void SetKey(unsigned char key) = 0;
	
	virtual void SetProtocal( bool bIsOldProtocal ) = 0;
};

extern "C" can_interface* CreateCanInterface(const char* can_name);

#endif
