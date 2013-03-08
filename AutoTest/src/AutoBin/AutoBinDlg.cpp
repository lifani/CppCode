
// AutoBinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoBin.h"
#include "AutoBinDlg.h"
#include "InputName.h"
#include <afxcmn.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoBinDlg 对话框




CAutoBinDlg::CAutoBinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoBinDlg::IDD, pParent), m_pFlowCtrl(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoBinDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE, m_treeCtrl);
    DDX_Control(pDX, IDC_COMBO_FILE, m_CcomboBox);
    DDX_Control(pDX, IDC_TREE_SHOW, m_ShowTreeCtrl);
}

BEGIN_MESSAGE_MAP(CAutoBinDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BTN_EXECUTE, &CAutoBinDlg::OnBnClickedBtnExecute)
    ON_BN_CLICKED(IDC_BTN_FILE, &CAutoBinDlg::OnBnClickedBtnFile)
    ON_BN_CLICKED(IDC_RADIO2, &CAutoBinDlg::OnBnClickedRadio2)
    ON_BN_CLICKED(IDC_RADIO1, &CAutoBinDlg::OnBnClickedRadio1)
    ON_CBN_SELCHANGE(IDC_COMBO_FILE, &CAutoBinDlg::OnCbnSelchangeComboFile)
    ON_WM_TIMER()
    ON_NOTIFY(NM_RCLICK, IDC_TREE, &CAutoBinDlg::OnNMRClickTree)
    ON_COMMAND(ID__ADD, &CAutoBinDlg::OnContextMenuAdd)
    ON_COMMAND(ID__DELETE, &CAutoBinDlg::OnContextMenuDelete)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CAutoBinDlg::OnBnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_STOP, &CAutoBinDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CAutoBinDlg 消息处理程序

BOOL CAutoBinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    InitImage();

    m_bRunning = FALSE;

	// TODO: 在此添加额外的初始化代码
    VERIFY(m_Font.CreateFont(
        24,                        // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        FW_NORMAL,                 // nWeight
        FALSE,                     // bItalic
        FALSE,                     // bUnderline
        0,                         // cStrikeOut
        ANSI_CHARSET,              // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        DEFAULT_QUALITY,           // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        _T("Arial")));             // lpszFacename

    this->GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
    this->GetDlgItem(IDC_BTN_STOP)->ShowWindow(FALSE);
    m_IsChanged = FALSE;

    m_pFlowCtrl = CFlowCtrl::Instance();
    m_pFlowCtrl->Init();

    m_strAppPath = m_pFlowCtrl->GetAppPath();

    m_strDllXmlPath = m_strAppPath + SEPERATOR + DLL_XML;
    m_strExeXmlPath = m_strAppPath + SEPERATOR + EXE_XML;
    m_strPathXmlPath = m_strAppPath + SEPERATOR + PATH_XML;

    // 默认选择dll
    CButton* pButton = (CButton*)this->GetDlgItem(IDC_RADIO1);
    pButton->SetCheck(1);

    // 输入文件路径
    vector<string> vPath;
    m_pFlowCtrl->ReadPath(m_strPathXmlPath, vPath);
    AddPath(vPath);

    // 测试对象树
    m_pFlowCtrl->ReadTree(m_strDllXmlPath, m_vTestElement);  

    m_hRoot = m_treeCtrl.InsertItem(_T("测试流程"));
    CreateTree(m_vTestElement);
    m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutoBinDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoBinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoBinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CAutoBinDlg::CreateTree( vector<TestElement>& vTestElement )
{
    USES_CONVERSION;

    vector<TestElement>::iterator itrEle = vTestElement.begin();
    for (; itrEle != vTestElement.end(); ++itrEle)
    {
        CString strDesc = A2T(itrEle->desc.c_str());
        HTREEITEM treeEle = m_treeCtrl.InsertItem(strDesc, m_hRoot);

        vector<TestAtom>::iterator itrAtom = itrEle->vTestAtom.begin();
        for (; itrAtom != itrEle->vTestAtom.end(); ++itrAtom)
        {
            CString strName = A2T(itrAtom->name.c_str());
            m_treeCtrl.InsertItem(strName, 3, 1, treeEle);
        }

        m_treeCtrl.Expand(treeEle, TVE_EXPAND);
    }

    return true;
}


void CAutoBinDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here

    if (m_IsChanged)
    {
        if (IDOK == AfxMessageBox(_T("是否保存已被改变的测试对象树？"), MB_OKCANCEL))
        {
            SaveTree();
        }
    }

    SavePath();

    if (NULL != m_pFlowCtrl)
    {
        delete m_pFlowCtrl;
        m_pFlowCtrl = NULL;
    }
}

void CAutoBinDlg::CreateTestPath()
{
    m_vTestPath.clear();

    m_MaxId = 0;
    
    HTREEITEM hTestElement = m_treeCtrl.GetChildItem(m_hRoot);
    while (hTestElement)
    {
        ++m_MaxId;
        HTREEITEM hTestAtom = m_treeCtrl.GetChildItem(hTestElement);
        while (hTestAtom)
        {
            if (m_treeCtrl.GetCheck(hTestAtom))
            {
                PushbackVTestPath(m_treeCtrl.GetItemText(hTestElement), 
                    m_treeCtrl.GetItemText(hTestAtom));
            }

            hTestAtom = m_treeCtrl.GetNextSiblingItem(hTestAtom);
        }

        hTestElement = m_treeCtrl.GetNextSiblingItem(hTestElement);
    }
}

void CAutoBinDlg::OnBnClickedBtnExecute()
{
    ClearInfo();

    CreateTestPath();

    CreateShowTree(m_vTestPath);

    if (m_vTestPath.empty())
    {
        AfxMessageBox(_T("请选择测试节点！"));
        return;
    }

    m_pFlowCtrl->StartTest(m_strPathIn, m_vTestPath);

    m_bRunning = TRUE;

    // 启动定时器
    SetTimer(0, 3000, 0);

    GetDlgItem(IDC_BTN_EXECUTE)->ShowWindow(FALSE);
    GetDlgItem(IDC_BTN_STOP)->ShowWindow(TRUE);
}

void CAutoBinDlg::PushbackVTestPath( CString& strDesc, CString& strName )
{
    USES_CONVERSION;
    string desc = T2A(strDesc.GetBuffer());
    string name = T2A(strName.GetBuffer());

    int id = 0;
    if (!m_vTestPath.empty())
    {
        TestElement& testEle = m_vTestPath.back();
        id = testEle.id;
    }

    vector<TestElement>::iterator itr = m_vTestElement.begin();
    for (; itr != m_vTestElement.end(); ++itr)
    {
        if (itr->id == id && itr->desc == desc)
        {
           CreateTestAtom(id, itr, name);
        }

        if (itr->id > id && itr->id <= m_MaxId)
        {
            if (itr->desc != desc)
            {
                m_vTestPath.push_back(*itr);
            }
            else
            {
                CreateTestAtom(id, itr, name);
                break;
            }   
        }
    }
}

void CAutoBinDlg::CreateTestAtom(int id, vector<TestElement>::iterator itr, string& strName )
{
    vector<TestAtom>::iterator itrAtom = itr->vTestAtom.begin();
    for (; itrAtom != itr->vTestAtom.end(); ++itrAtom)
    {
        if (itrAtom->name == strName)
        {
            break;
        }
    }

    if (itr->id > id)
    {
        TestElement testEle;
        testEle.desc = itr->desc;
        testEle.id = itr->id;
        testEle.vTestAtom.push_back(*itrAtom);

        m_vTestPath.push_back(testEle);
    }
    else
    {
        TestElement& tEle = m_vTestPath.back();
        tEle.vTestAtom.push_back(*itrAtom);
    }
    
}

void CAutoBinDlg::OnBnClickedBtnFile()
{
    CFileDialog dialog(TRUE);
   
    if (IDOK == dialog.DoModal())
    {
        CString strPath = dialog.GetPathName();

        int cursel = m_CcomboBox.AddString(strPath);
        m_CcomboBox.SetCurSel(cursel);

        USES_CONVERSION;

        m_strPathIn = T2A(strPath.GetBuffer());
    }

    UpdateData(TRUE);
}

void CAutoBinDlg::SavePath()
{
    vector<string> vPath;

    USES_CONVERSION;

    int cnt = 0;
    for (int i = m_CcomboBox.GetCount() - 1; i >= 0 && cnt < 5; --i, ++cnt)
    {
        CString str;
        m_CcomboBox.GetLBText(i, str);

        vPath.push_back(T2A(str.GetBuffer()));
    }

    string strPath = m_strAppPath + SEPERATOR + PATH_XML;
    m_pFlowCtrl->SavePath(strPath, vPath);
}

void CAutoBinDlg::AddPath( vector<string>& vPath )
{
    USES_CONVERSION;

    vector<string>::iterator itr = vPath.begin();
    for (; itr != vPath.end(); ++itr)
    {
        CString str = A2T(itr->c_str());
        m_CcomboBox.AddString(str);
    }

    m_CcomboBox.SetCurSel(0);

    GetPath();
}

void CAutoBinDlg::OnBnClickedRadio2()
{
    m_treeCtrl.DeleteAllItems();

    m_hRoot = m_treeCtrl.InsertItem(_T("测试流程"));

    m_pFlowCtrl->ReadTree(m_strExeXmlPath, m_vTestElement);
    CreateTree(m_vTestElement);
    m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
}

void CAutoBinDlg::OnBnClickedRadio1()
{
    m_treeCtrl.DeleteAllItems();

    m_hRoot = m_treeCtrl.InsertItem(_T("测试流程"));

    m_pFlowCtrl->ReadTree(m_strDllXmlPath, m_vTestElement);
    CreateTree(m_vTestElement);
    m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
}

void CAutoBinDlg::OnCbnSelchangeComboFile()
{
    GetPath();
}

void CAutoBinDlg::GetPath()
{
    int cursel = m_CcomboBox.GetCurSel();

    CString str;
    m_CcomboBox.GetLBText(cursel, str);

    USES_CONVERSION;
    m_strPathIn = T2A(str.GetBuffer());
}

void CAutoBinDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    m_pFlowCtrl->GetProgress(m_vTestPath);

    BOOL bRunning = FALSE;
    if (!m_vTestPath.empty())
    {
        vector<TestElement>::iterator itrEle = m_vTestPath.begin();
        for (; itrEle != m_vTestPath.end(); ++itrEle)
        {
            vector<TestAtom>::iterator itr = itrEle->vTestAtom.begin();
            for (; itr != itrEle->vTestAtom.end(); ++itr)
            {
                if (itr->nResult == 0)
                {
                    bRunning = TRUE;

                    USES_CONVERSION;
                    m_ProgressDesc.Format(_T("%s %s"), 
                        _T("测试进行中，当前运行："),  A2T(itrEle->desc.c_str()));

                    // 跳出循环
                    break;
                }
            }

            // 跳出循环
            if (bRunning)
            {
                break;
            }
        }
    }

    // 更新运行状态

    if (!bRunning)
    {
        KillTimer(0);

        m_bRunning = FALSE;

        m_ProgressDesc.Format(_T("%s"), _T("测试已完成！"));

        GetDlgItem(IDC_BTN_EXECUTE)->ShowWindow(TRUE);
        GetDlgItem(IDC_BTN_STOP)->ShowWindow(FALSE);
    }

    ShowInfo();

    CDialog::OnTimer(nIDEvent);
}

void CAutoBinDlg::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    if (m_bRunning)
    {
        return;
    }

    CPoint pt;
    GetCursorPos(&pt);

    UINT uFlgs;
    m_treeCtrl.ScreenToClient(&pt);

    m_hItem = m_treeCtrl.HitTest(pt, &uFlgs);
    if (NULL != m_hItem)
    {
        CMenu menu;
        if (menu.LoadMenu(IDR_MENU1))
        {
            m_pSubMenu = menu.GetSubMenu(0);

            CPoint mPoint;
            GetCursorPos(&mPoint);

            EnableMenuItem();
            m_pSubMenu->TrackPopupMenu(uFlgs, mPoint.x, mPoint.y, this);
        }
    }

    *pResult = 0;
}

void CAutoBinDlg::OnContextMenuAdd()
{
    USES_CONVERSION;

    CString strName;
    if (m_hItem == m_hRoot)
    {
        CInputName Inputdlg;
        if (IDOK != Inputdlg.DoModal())
        {
            return;
        }
        
        strName = Inputdlg.m_strName.Trim();
    }
    else
    {
        CFileDialog dialog(TRUE);
        if (IDOK == dialog.DoModal())
        {
             strName = dialog.GetFileName();

             CString strNewFileName;
             strNewFileName.Format(_T("%s%s%s"), A2T(m_strAppPath.c_str()), _T(SEPERATOR), strName);
             CString strExistFileName = dialog.GetPathName();
             if (!CopyFile(strExistFileName, strNewFileName, TRUE))
             {
                 AfxMessageBox(_T("该节点已经存在！"));
                 return;
             }
        }
    }

    if (!strName.IsEmpty())
    {
        m_treeCtrl.InsertItem(strName, m_hItem);

        m_IsChanged = TRUE;
        this->GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
        this->GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(FALSE);

        m_treeCtrl.Expand(m_hItem, TVE_EXPAND);
    }
}

void CAutoBinDlg::OnContextMenuDelete()
{
    USES_CONVERSION;

    if (NULL != m_hItem)
    {
        CString strName = m_treeCtrl.GetItemText(m_hItem);
        CString strExistName;
        strExistName.Format(_T("%s%s%s"), A2T(m_strAppPath.c_str()), _T(SEPERATOR), strName);

        DeleteFile(strExistName);

        m_treeCtrl.DeleteItem(m_hItem);

        m_IsChanged = TRUE;
        this->GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
        this->GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(FALSE);
    }
}

void CAutoBinDlg::EnableMenuItem()
{
    if (m_hItem == m_hRoot || m_treeCtrl.ItemHasChildren(m_hItem))
    {
        m_pSubMenu->EnableMenuItem(ID__DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    if (!CanPopMenu(m_hItem))
    {
        m_pSubMenu->EnableMenuItem(ID__ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }
}

bool CAutoBinDlg::CanPopMenu( HTREEITEM hItem )
{
    HTREEITEM hPItem = m_treeCtrl.GetParentItem(hItem);
    if (NULL == hPItem || hPItem == m_hRoot)
    {
        return true;
    }

    return false;
}

void CAutoBinDlg::OnBnClickedBtnSave()
{
    SaveTree();

    m_IsChanged = FALSE;
    this->GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
    this->GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(TRUE);
}

void CAutoBinDlg::SaveTree()
{
    USES_CONVERSION;

    m_vTestElement.clear();

    int i = 0;

    HTREEITEM hEle = m_treeCtrl.GetChildItem(m_hRoot);
    while (hEle)
    {
        ++i;

        TestElement tEle;
        tEle.id = i;

        CString strDesc = m_treeCtrl.GetItemText(hEle);
        tEle.desc = T2A(strDesc.GetBuffer());

        HTREEITEM hAtom = m_treeCtrl.GetChildItem(hEle);
        while (hAtom)
        {
            TestAtom tAtom;
            tAtom.id = i;
            tAtom.nResult = 0;

            CString strName = m_treeCtrl.GetItemText(hAtom);
            tAtom.name = T2A(strName.GetBuffer());

            tEle.vTestAtom.push_back(tAtom);

            hAtom = m_treeCtrl.GetNextSiblingItem(hAtom);
        }

        m_vTestElement.push_back(tEle);

        hEle = m_treeCtrl.GetNextSiblingItem(hEle);
    }

    CButton* pButton = (CButton*)this->GetDlgItem(IDC_RADIO1);
    if (pButton->GetCheck() == 1)
    {
        m_pFlowCtrl->SaveTree(m_strDllXmlPath, m_vTestElement);
    }
    else
    {
        m_pFlowCtrl->SaveTree(m_strExeXmlPath, m_vTestElement);
    }
    
}

void CAutoBinDlg::OnBnClickedBtnStop()
{
    m_pFlowCtrl->EndTest();

    m_bRunning = FALSE;
    KillTimer(0);

    m_ProgressDesc.Format(_T("%s"), _T("测试已被停止！"));
    ShowInfo();

    GetDlgItem(IDC_BTN_EXECUTE)->ShowWindow(TRUE);
    GetDlgItem(IDC_BTN_STOP)->ShowWindow(FALSE);
}

void CAutoBinDlg::ShowInfo()
{
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(m_ProgressDesc);
    GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_Font);

    UpdateShowTree();
}

void CAutoBinDlg::ClearInfo()
{
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(_T(""));
}

bool CAutoBinDlg::CreateShowTree( vector<TestElement>& vTestPath )
{
    m_ShowTreeCtrl.DeleteAllItems();

    m_hShowRoot = m_ShowTreeCtrl.InsertItem(_T("测试流程"), 2, 2);

    USES_CONVERSION;

    CString strLastDesc = _T("");
    HTREEITEM lastItem;

    vector<TestElement>::iterator itr = m_vTestPath.begin();
    for (; itr != m_vTestPath.end(); ++itr)
    {
        CString strDesc = A2T(itr->desc.c_str());
        if (strDesc.Compare(strLastDesc) != 0)
        {
            lastItem = m_ShowTreeCtrl.InsertItem(strDesc, 2, 2, m_hShowRoot);
            itr->p = (void*)lastItem;

            strLastDesc = strDesc;
        }

        vector<TestAtom>::iterator itrAtom = itr->vTestAtom.begin();
        for (; itrAtom != itr->vTestAtom.end(); ++itrAtom)
        {
            CString strName = A2T(itrAtom->name.c_str());
            HTREEITEM h = m_ShowTreeCtrl.InsertItem(strName, 2, 2, lastItem);

            itrAtom->p = (void*)h;
        }

        m_ShowTreeCtrl.Expand(lastItem, TVE_EXPAND);
    }

    m_ShowTreeCtrl.Expand(m_hShowRoot, TVE_EXPAND);

    return true;
}

void CAutoBinDlg::LookUpMaxId()
{
    m_MaxId = 0;

    HTREEITEM hTestElement = m_treeCtrl.GetChildItem(m_hRoot);
    while (hTestElement)
    {
        HTREEITEM hTestAtom = m_treeCtrl.GetChildItem(hTestElement);
        while (hTestAtom)
        {
            if (m_treeCtrl.GetCheck(hTestAtom))
            {
                ++m_MaxId;
                break;
            }

            hTestAtom = m_treeCtrl.GetNextSiblingItem(hTestAtom);
        }

        hTestElement = m_treeCtrl.GetNextSiblingItem(hTestElement);
    }
}

void CAutoBinDlg::UpdateShowTree()
{
    USES_CONVERSION;

    bool bRunning = false;
    int index = 0;

    vector<TestElement>::iterator itr = m_vTestPath.begin();
    for (; itr != m_vTestPath.end(); ++itr)
    {
        vector<TestAtom>::iterator itrAtom = itr->vTestAtom.begin();
        for (; itrAtom != itr->vTestAtom.end(); ++itrAtom)
        {
            CString strDesc;
            strDesc.Format(_T("%s  %d ms"), A2T(itrAtom->name.c_str()), itrAtom->nResult);
            m_ShowTreeCtrl.SetItemText((HTREEITEM)itrAtom->p, strDesc);

            index = itrAtom->nResult > 0 ? 0 : itrAtom->nResult == 0 ? 1 : 3;
            SetItemImage((HTREEITEM)itrAtom->p, index);
        }

        if (itr->bRunning)
        {
            bRunning = true;
        }

        index = itr->bRunning ? 1 : 0;
        SetItemImage((HTREEITEM)itr->p, index);
    }

    index = bRunning ? 1 : 0;
    SetItemImage(m_hShowRoot, index);
}

void CAutoBinDlg::InitImage()
{
    HICON icon[4];
    icon[0] = AfxGetApp()->LoadIcon(IDI_ICON1);
    icon[1] = AfxGetApp()->LoadIcon(IDI_ICON2);
    icon[2] = AfxGetApp()->LoadIcon(IDI_ICON3);
    icon[3] = AfxGetApp()->LoadIcon(IDI_ICON4);

    m_ImageList.Create(16, 16, 0, 4, 4);
    for (int i = 0; i < 4; i++)
    {
        m_ImageList.Add(icon[i]);
    }

    m_ShowTreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
}

void CAutoBinDlg::SetItemImage( HTREEITEM hItem, int index )
{
    m_ShowTreeCtrl.SetItemImage(hItem, index, index);
}