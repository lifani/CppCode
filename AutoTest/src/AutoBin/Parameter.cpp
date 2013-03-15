// Parameter.cpp : implementation file
//

#include "stdafx.h"
#include "AutoBin.h"
#include "Parameter.h"

// CParameter dialog

IMPLEMENT_DYNAMIC(CParameter, CDialog)

CParameter::CParameter(CWnd* pParent /*=NULL*/)
	: CDialog(CParameter::IDD, pParent)
{

}

CParameter::~CParameter()
{
}

void CParameter::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PARA, m_listPara);
}


BEGIN_MESSAGE_MAP(CParameter, CDialog)
    ON_BN_CLICKED(IDC_BTN_ADD, &CParameter::OnBnClickedBtnAdd)
    ON_BN_CLICKED(IDC_BTN_DELETE, &CParameter::OnBnClickedBtnDelete)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CParameter::OnBnClickedBtnSave)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CParameter::OnInitDialog()
{
    CDialog::OnInitDialog();

    CRect rc;
    m_listPara.GetClientRect(rc);
    m_listPara.InsertColumn(0, _T("参数"), LVCFMT_LEFT, rc.Width());

    m_xml.ReadArgXml(m_strPath, m_vPara);

    vector<string>::iterator itr = m_vPara.begin();
    for (; itr != m_vPara.end(); ++itr)
    {
        m_listPara.InsertItem(0, CString(itr->c_str()));
    }

    return TRUE;
}


// CParameter message handlers

void CParameter::OnBnClickedBtnAdd()
{
    CString strArg;
    GetDlgItem(IDC_EDIT_PARA)->GetWindowText(strArg);

    if (!strArg.IsEmpty())
    {
        m_listPara.InsertItem(0, strArg);
        GetDlgItem(IDC_EDIT_PARA)->SetWindowText(_T(""));
    }
}

void CParameter::OnBnClickedBtnDelete()
{
    POSITION pos = m_listPara.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        AfxMessageBox(_T("请选择删除项！"));
        return;
    }

    int item = m_listPara.GetNextSelectedItem(pos);
    m_listPara.DeleteItem(item);
}

void CParameter::OnBnClickedBtnSave()
{
    m_vPara.clear();

    USES_CONVERSION;

    UINT count = m_listPara.GetItemCount();
    for (int i = 0; i < count; ++i)
    {
        CString strArg = m_listPara.GetItemText(i, 0);

        m_vPara.push_back(T2A(strArg.GetBuffer()));
    }

    m_xml.WriteArgXml(m_strPath, m_vPara);
}

void CParameter::OnDestroy()
{
    CDialog::OnDestroy();

    delete this;
}
