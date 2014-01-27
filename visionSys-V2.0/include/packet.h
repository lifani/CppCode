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
	
	int m_nLen; // ĳ������İ��ľ���ĳ��ȣ�����head�����len�ֶξ���
	int m_nPos; // ��ǰ�Ѿ���ȡ�������ݵĳ���
	
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
