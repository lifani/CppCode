// Detail.cpp : implementation file
//

#include "stdafx.h"
#include "AutoBin.h"
#include "Detail.h"


// CDetail dialog

IMPLEMENT_DYNAMIC(CDetail, CDialog)

CDetail::CDetail(CWnd* pParent /*=NULL*/)
	: CDialog(CDetail::IDD, pParent), m_pDetail(NULL)
{

}

CDetail::~CDetail()
{
}

void CDetail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDetail, CDialog)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDetail::OnInitDialog()
{
    return TRUE;
}

void CDetail::SetDetail( map<string, int>* pDetail )
{
    m_pDetail = pDetail;
}


// CDetail message handlers

void CDetail::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    delete this;
}
