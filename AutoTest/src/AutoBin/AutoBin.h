
// AutoBin.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoBinApp:
// �йش����ʵ�֣������ AutoBin.cpp
//

class CAutoBinApp : public CWinAppEx
{
public:
	CAutoBinApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoBinApp theApp;