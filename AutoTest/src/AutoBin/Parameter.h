#pragma once
#include "afxcmn.h"

#include "../../include/typedef.h"
#include "../xml/xml.h"

// CParameter dialog

class CParameter : public CDialog
{
	DECLARE_DYNAMIC(CParameter)

public:
	CParameter(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParameter();

// Dialog Data
	enum { IDD = IDD_DIALOG_PARA };

    BOOL OnInitDialog();

    void SetPara(const string& strPath)
    {
        m_strPath = strPath;
    }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_listPara;

private:

    string m_strPath;
    CXml m_xml;

    vector<string> m_vPara;
public:
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnDelete();
    afx_msg void OnBnClickedBtnSave();
    afx_msg void OnDestroy();
};
