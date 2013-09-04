#ifndef __CAN_INTERFACE__
#define __CAN_INTERFACE__

class can_interface
{
public :
	
	virtual ~can_interface()
	{
	}
	
	virtual int Read(char* ptr, unsigned short* len) = 0;
	
	virtual int Write(const char* ptr, unsigned int len) = 0;

	virtual bool Register() = 0;
	
	virtual bool ResetFilter(unsigned short can_id, unsigned short cmd_code) = 0;
	
	virtual void SetKey(unsigned char key) = 0;
};

extern "C" can_interface* CreateCanInterface(const char* can_name, unsigned short can_id, unsigned short cmd_code, unsigned int size);

#endif
