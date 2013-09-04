#ifndef __CAN_DATA__
#define __CAN_DATA__

#include "../../../include/typedef.h"
#include "../../../include/can_interface.h"

typedef struct _can_head
{
	unsigned char head[4];
	unsigned short cmd_code;
	unsigned short data_len;
} can_head;

const unsigned char can_tail[4] = {0x66, 0xcc, 0x66, 0xcc};

class can_data : public can_interface
{
public :
	can_data(const char* can_name, unsigned short cmd_id, unsigned short cmd_code, unsigned int size);
	
	virtual ~can_data();
	
	virtual int Read(char* ptr, unsigned short* len);
	
	virtual int Write(const char* ptr, unsigned int len);
	
	virtual bool Register();
	
	virtual bool ResetFilter(unsigned short can_id, unsigned short cmd_code);
	
	virtual void SetKey(unsigned char key);

private :
	
	bool SetFilter(unsigned short can_id, unsigned short cmd_code);
	
	bool IsHead(struct can_frame& frame);

	bool IsTail(struct can_frame& frame, int pos);
	
	bool IsNeed(struct can_frame& frame);
	
private :

	string m_can_name;
	
	unsigned short m_can_id;
	unsigned short m_cmd_code;
	unsigned int m_size;
	
	const int m_bitrate;
	
	unsigned char m_key;
	
	int m_can_sck;
	
};

#endif
