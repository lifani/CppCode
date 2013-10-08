#ifndef __CAN_INTERFACE__
#define __CAN_INTERFACE__

class can_interface
{
public :
	
	virtual ~can_interface()
	{
	}
	
	virtual int Read(char* ptr, unsigned short len, int type = 0) = 0;
	
	virtual int Write(const char* ptr, unsigned int len, unsigned short can_id = 0, unsigned short cmd_code = 0) = 0;

	virtual bool Init() = 0;
	
	virtual bool SetFilter(unsigned short can_id, unsigned short cmd_code) = 0;
	
	virtual void SetKey(unsigned char key) = 0;
	
	virtual void SetProtocal( bool bIsOldProtocal ) = 0;
};

extern "C" can_interface* CreateCanInterface(const char* can_name);

#endif
