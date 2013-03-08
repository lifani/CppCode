
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

    bool CreateShowTree(vector<TestElement>& vTestPath);

    void CreateTestPath();

    void PushbackVTestPath(CString& strDesc, CString& strName);

    void CreateTestAtom(int id, vector<TestElement>::iterator itr, string& strName);

    void SavePath();

    void AddPath(vector<string>& vPath);

    void GetPath();

    void EnableMenuItem();

    bool CanPopMenu(HTREEITEM hItem);

    void SaveTree();

    void InitImage();

    void ShowInfo();

    void ClearInfo();

    void LookUpMaxId();

    void UpdateShowTree();

    void SetItemImage(HTREEITEM hItem, int index);

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
    CTreeCtrl m_ShowTreeCtrl;

    CComboBox m_CcomboBox;

    CFlowCtrl* m_pFlowCtrl;

    vector<TestElement> m_vTestElement;
    vector<TestElement> m_vTestPath;

    HTREEITEM m_hRoot;
    HTREEITEM m_hItem;
    CMenu* m_pSubMenu;

    HTREEITEM m_hShowRoot;

    string m_strAppPath;
    string m_strPathIn;

    string m_strDllXmlPath;
    string m_strExeXmlPath;
    string m_strPathXmlPath;

    BOOL m_IsChanged;

    BOOL m_bRunning;
    CString m_ProgressDesc;

    CFont m_Font;

    int m_MaxId;

    CImageList m_ImageList;


    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBtnExecute();
    afx_msg void OnBnClickedBtnFile();
    
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnCbnSelchangeComboFile();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenuAdd();
    afx_msg void OnContextMenuDelete();
    afx_msg void OnBnClickedBtnSave();
    afx_msg void OnBnClickedBtnStop();
};
