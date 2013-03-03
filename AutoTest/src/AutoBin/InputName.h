#pragma once


// CInputName dialog

class CInputName : public CDialog
{
	DECLARE_DYNAMIC(CInputName)

public:
	CInputName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputName();

// Dialog Data
	enum { IDD = IDD_INPUT_DIALOG };

    CString m_strName;

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
};
