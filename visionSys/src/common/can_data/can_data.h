#ifndef __CAN_DATA__
#define __CAN_DATA__

#include <typedef.h>
#include <can_interface.h>
#include <datatype.h>

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
	can_data(const char* can_name);
	
	virtual ~can_data();
	
	virtual int Read(char* ptr, unsigned short len, int type = 0);
	
	virtual int Write(const char* ptr, unsigned int len,unsigned short can_id = 0, unsigned short cmd_code = 0);
	
	virtual bool Init();
	
	virtual bool SetFilter(unsigned short can_id, unsigned short cmd_code);
	
	virtual void SetKey(unsigned char key);
	
	virtual void SetProtocal( bool bIsOldProtocal );

private :
	
	bool IsHead(struct can_frame& frame);

	bool IsTail(struct can_frame& frame, int pos);
	
	bool IsNeed(struct can_frame& frame);
	
	bool Check4Q(char* pData);
	
private :

	string m_can_name;
	
	unsigned short m_can_id;
	unsigned short m_cmd_code;
	
	const int m_bitrate;
	
	unsigned char m_key;
	
	int m_can_sck;
	
	char m_SndData[1024];
	
	bool m_bIsOldProtocal;
};

#endif
