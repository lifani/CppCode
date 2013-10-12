#ifndef __PACKET__
#define __PACKET__

#include <can_interface.h>

#define MAXPACKETSIZE 1024

class can_frame;

class Packet
{
public :
	Packet( int nCanId, int nCmdCode );
	
	virtual ~Packet();
	
	virtual bool Push( can_frame &rFrame );

	unsigned char* GetBuffer();

	void Reset();

	int GetCanId();

	bool IsEqualFilter( unsigned short usCanId );

	bool IsValid();

	void SetKey( unsigned char ucKey );

private :
	
	int m_nLen; // 某个具体的包的具体的长度，根据head里面的len字段决定
	int m_nPos; // 当前已经读取到的数据的长度
	
	unsigned short m_usCanId;
	unsigned short m_usCmdCode;
	
	unsigned char m_pBuffer[MAXPACKETSIZE];
	bool	m_bEmpty;

	unsigned char s_ucKey;
private:
	
	bool IsHead(struct can_frame& frame);

	bool IsTail(struct can_frame& frame, int pos);

	bool Check4Q( unsigned char* pData);
};

#endif
