// InputName.cpp : implementation file
//

#include "stdafx.h"
#include "AutoBin.h"
#include "InputName.h"


// CInputName dialog

IMPLEMENT_DYNAMIC(CInputName, CDialog)

CInputName::CInputName(CWnd* pParent /*=NULL*/)
	: CDialog(CInputName::IDD, pParent)
{

}

CInputName::~CInputName()
{
}

void CInputName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputName, CDialog)
    ON_BN_CLICKED(IDOK, &CInputName::OnBnClickedOk)
END_MESSAGE_MAP()


// CInputName message handlers

void CInputName::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    this->GetDlgItemText(IDC_EDIT_NAME, m_strName);

    OnOK();
}

BOOL CInputName::OnInitDialog()
{
    CDialog::OnInitDialog();

    this->GetDlgItem(IDC_EDIT_NAME)->SetFocus();

    return TRUE;
}
