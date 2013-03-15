#pragma once


// CDetail dialog

class CDetail : public CDialog
{
	DECLARE_DYNAMIC(CDetail)

public:
	CDetail(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDetail();

// Dialog Data
	enum { IDD = IDD_DETAIL };

    BOOL OnInitDialog();

    void SetDetail(map<string, int>* pDetail);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:

    map<string, int>* m_pDetail;
public:
    afx_msg void OnDestroy();
};
