
// AutoBinDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "..\..\third\treeCtrl\CustomDrawControl.h"
#include "afxwin.h"

// CAutoBinDlg 对话框
class CAutoBinDlg : public CDialog
{
// 构造
public:
	CAutoBinDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOBIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


private:

    bool CreateTree(vector<TestElement>& vTestElement);

    void CreateTestPath();

    void PushbackVTestPath(CString& strDesc, CString& strName);

    void CreateTestAtom(vector<TestElement>::iterator itr, string& strName);

    void SavePath();

    void AddPath(vector<string>& vPath);

    void GetPath();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CMultiSelTriCheckTreeCtrl m_treeCtrl;
    CComboBox m_CcomboBox;

    CFlowCtrl* m_pFlowCtrl;

    vector<TestElement> m_vTestElement;
    vector<TestElement> m_vTestPath;

    HTREEITEM m_hRoot;

    string m_strAppPath;
    string m_strPathIn;

    string m_strDllXmlPath;
    string m_strExeXmlPath;
    string m_strPathXmlPath;

    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBtnExecute();
    afx_msg void OnBnClickedBtnFile();
    
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnCbnSelchangeComboFile();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
