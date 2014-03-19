#include "sndCanCtrl.h"

#include "MF_CRC8.h"
#include "crc16.h"

CSndCanCtrl::CSndCanCtrl() : m_pos(0), m_size(0), m_sndCtrl(sizeof(SND_DATA), 10, true)
{
}

CSndCanCtrl::~CSndCanCtrl()
{
	LOGE("CSndCanCtrl destroy.");
}

int CSndCanCtrl::Initialize(unsigned short canId, unsigned short cmd)
{
	CAbstractCanCtrl::Initialize(canId, cmd);
	
	if (-1 == m_sndCtrl.Initialize())
	{
		return -1;
	}
	
	return 0;
}

int CSndCanCtrl::Process(struct can_frame* pFrame)
{
	if (NULL == pFrame)
	{
		return -1;
	}
	
	if (0 == m_size && 0 != m_sndCtrl.pop((char*)&m_sndData))
	{
		return 0;
	}
	
	m_size = m_sndData.size;
	
	pFrame->can_id = m_canId;

	unsigned int i = 0;
	for (; i < FRAME_LEN && m_pos < m_size; ++i, ++m_pos)
	{
		pFrame->data[i] = m_sndData.szData[m_pos];
	}
	
	pFrame->can_dlc = i;
	
	if (m_pos == m_size)
	{
		m_pos = 0;
		m_size = 0;
	}
	
	return pFrame->can_dlc;
}

int CSndCanCtrl::SetContent(char* ptr, int len)
{
	if (NULL == ptr || 0 == len)
	{
		return -1;
	}
	
	SND_DATA sndData;
	
	sndData.szData[0] = 0x55;
	
	unsigned short uLen = (unsigned)len + 8;
	unsigned short *pLen = (unsigned short*)(sndData.szData + 1);
	
	*pLen = uLen;

	unsigned short *pCmd = (unsigned short*)(sndData.szData + 4);
	*pCmd = m_cmdCode;
	
	memcpy(sndData.szData + 6, ptr, len);
	
	Append_CRC8_Check_Sum((unsigned char*)sndData.szData, 4);
	Append_CRC16_Check_Sum((unsigned char*)sndData.szData, uLen);
	
	sndData.size = (unsigned int)uLen;
	
	return m_sndCtrl.push((char*)&sndData);
}

