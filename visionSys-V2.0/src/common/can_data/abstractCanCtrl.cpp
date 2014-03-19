/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "abstractCanCtrl.h"

CAbstractCanCtrl::CAbstractCanCtrl()
: m_begin(false), m_canId(0), m_cmdCode(0), m_size(0), m_pos(0), m_key(0)
{
}

CAbstractCanCtrl::~CAbstractCanCtrl()
{
	LOGE("CAbstractCanCtrl destroy.");
}

/************************************
���ܣ�	CAN�����ʼ�����ɸ���
������	pFrame struct can_frame* CAN����֡
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CAbstractCanCtrl::Initialize(unsigned short canId, unsigned short cmd)
{	
	m_uCanHead.szHead[0] = 0x55;
	m_uCanHead.szHead[1] = 0xaa;
	m_uCanHead.szHead[2] = 0x55;
	m_uCanHead.szHead[3] = 0xaa;
	
	m_uCanTail.szTail[0] = 0x66;
	m_uCanTail.szTail[1] = 0xcc;
	m_uCanTail.szTail[2] = 0x66;
	m_uCanTail.szTail[3] = 0xcc;
	
	m_canId = canId;
	m_cmdCode = cmd;
	
	return 0;
}

/************************************
���ܣ�	����֡������
������	pFrame struct can_frame* CAN����֡
���أ�	�ɹ� > 0, ʧ�� -1
************************************/
int CAbstractCanCtrl::Process(struct can_frame* pFrame)
{
	int err = -1;
	if (CheckHead((char*)pFrame->data, pFrame->can_dlc))
	{
		m_size = (unsigned int)*(unsigned short*)(pFrame->data + 6);
		if (m_size > sizeof(m_buf))
		{
			return err;
		}
		
		m_pos = 0;
		m_begin = true;
		
		return err;
	}
	
	if (m_begin)
	{
		for (int i = 0; i < pFrame->can_dlc && m_pos < m_size + 4; ++i, ++m_pos)
		{
			m_buf[m_pos] = pFrame->data[i];
		}
		
		if (m_pos == m_size + 4)
		{
			err = CheckTotal(m_buf + m_size, 4) ? 0 : -1;
			m_begin = false;
		}
	}
	
	return err;
}

/************************************
���ܣ�	��ȡ����
������	ptr char*& ����ָ������
���أ�	���ݳ���
************************************/
int CAbstractCanCtrl::GetContent(char*& ptr)
{
	ptr = m_buf;
	
	return m_size;
}

/************************************
���ܣ�	��������
������	ptr char* ����ָ��
		len int ���ݳ���
���أ�	�ɹ� 0�� ʧ�� -1
************************************/
int CAbstractCanCtrl::SetContent(char* ptr, int len)
{
	return 0;
}

/************************************
���ܣ�	CAN����֡��ͷУ�麯��
������	��
���أ�	�ɹ� true, ʧ�� false
************************************/
bool CAbstractCanCtrl::CheckHead(char* ptr, int len)
{
	unsigned int uHead = *(unsigned int*)(ptr);
	
	return m_uCanHead.uHead == uHead;
}

/************************************
���ܣ�	CAN����֡У�麯��
������	��
���أ�	�ɹ� true, ʧ�� false
************************************/
bool CAbstractCanCtrl::CheckTotal(char* ptr, int len)
{
	unsigned int uTail = *(unsigned int*)(ptr);
	
	return m_uCanTail.uTail == uTail;
}

/************************************
���ܣ�	CAN�����ʼ��
������	pFrame struct can_frame* CAN����֡
���أ�	��
************************************/
void CAbstractCanCtrl::SetKey(int key)
{
	m_key = key;
}
