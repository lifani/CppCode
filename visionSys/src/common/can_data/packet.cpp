#include "packet.h"
#include <poll.h>
#include <limits.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <errno.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <typedef.h>
#include <datatype.h>

#include <stdio.h>

Packet::Packet( int nCanId, int nCmdCode )
: m_nLen(0)
, m_nPos(0)
, m_usCanId(nCanId)
, m_usCmdCode(nCmdCode)
, m_bEmpty(true)
, s_ucKey(0)
{
	
}

Packet::~Packet()
{
	m_nLen = 0;
	m_nPos = 0;
	m_usCanId = 0;
	m_usCmdCode = 0;
	m_bEmpty = true;
}

bool Packet::IsEqualFilter( unsigned short usCanId )
{
	if ( m_usCanId == usCanId )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Packet::IsValid()
{
	if ( CAN_ID_ATTI == m_usCanId && CMD_CODE_ATTI == m_usCmdCode )
	{
		if ( Check4Q(m_pBuffer) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

bool Packet::Push( can_frame &rFrame )
{
	bool bIsComplete = false;
	if ( m_bEmpty )
	{
		unsigned short usCmdCode = *(unsigned short*)(rFrame.data + 4);
		if ( IsHead(rFrame) && m_usCmdCode == usCmdCode )
		{
			m_nLen = *(unsigned short*)((char*)rFrame.data + 6);
			m_bEmpty = false;
			m_nPos = 0;

		}
	}
	else
	{
		int i = 0;
		for (; i < rFrame.can_dlc && m_nPos < m_nLen; ++i, ++m_nPos)
		{
			*(m_pBuffer + m_nPos) = rFrame.data[i] ^ s_ucKey;
		}

		if (m_nPos == m_nLen && i < rFrame.can_dlc)
		{
			if (IsTail(rFrame, i))
			{
				if( IsValid() )
				{
					bIsComplete = true;
					m_bEmpty = true;
				}
				else
				{
					m_nPos = 0; //非法的包，重新把m_nPos置为0
					m_bEmpty = true;
				}
			}
			else
			{
				m_nPos = 0;
				m_bEmpty = true;
			}
		}
	}

	return bIsComplete;
}

unsigned char* Packet::GetBuffer()
{
	Reset();
	return m_pBuffer;
}

void Packet::Reset()
{
	m_nPos = 0;
	m_bEmpty = true;
}

int Packet::GetCanId()
{
	return m_usCanId;
}

void Packet::SetKey( unsigned char ucKey )
{
	s_ucKey = ucKey;
}

inline bool Packet::IsHead(struct can_frame& frame)
{
	bool bRet = false;

	if (frame.can_dlc == 8)
	{
		char* pData = (char*)frame.data;
		bRet = pData[0] == 0x55 && pData[1] == 0xaa && pData[2] == 0x55 && pData[3] == 0xaa;
	}

	return bRet;
}

inline bool Packet::IsTail(struct can_frame& frame, int pos)
{
	bool bRet = false;

	if (frame.can_dlc - pos >= 4)
	{
		char* pData = (char*)frame.data + (frame.can_dlc - 4);
		bRet = pData[0] == 0x66 && pData[1] == 0xcc && pData[2] == 0x66 && pData[3] == 0xcc;
	}

	return bRet;
}

bool Packet::Check4Q( unsigned char* pData)
{
	if (NULL == pData)
	{
		return false;
	}
	
	imu_body* p = (imu_body*)pData;
	
	if (p->q0 >= 1.0 || p->q1 >= 1.0 || p->q2 >= 1.0 || p->q3 >= 1.0)
	{
		return false;
	}
	
	if (p->q0 <= -1.0 || p->q1 <= -1.0 || p->q2 <= -1.0 || p->q3 <= -1.0)
	{
		return false;
	}
	
	float t = p->q0 * p->q0 + p->q1 * p->q1 + p->q2 * p-> q2 + p->q3 * p->q3;
	if (t >= 0.95 && t <= 1.05)
	{
		return true;
	}
	
	return false;
}
