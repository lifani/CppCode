
// AutoBinDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoBin.h"
#include "AutoBinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CAutoBinDlg �Ի���




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
END_MESSAGE_MAP()


// CAutoBinDlg ��Ϣ�������

BOOL CAutoBinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

    m_pFlowCtrl = CFlowCtrl::Instance();
    m_pFlowCtrl->Init();

    m_strAppPath = m_pFlowCtrl->GetAppPath();

    m_strDllXmlPath = m_strAppPath + SEPERATOR + DLL_XML;
    m_strExeXmlPath = m_strAppPath + SEPERATOR + EXE_XML;
    m_strPathXmlPath = m_strAppPath + SEPERATOR + PATH_XML;

    // Ĭ��ѡ��dll
    CButton* pButton = (CButton*)this->GetDlgItem(IDC_RADIO1);
    pButton->SetCheck(1);

    // �����ļ�·��
    vector<string> vPath;
    m_pFlowCtrl->ReadPath(m_strPathXmlPath, vPath);
    AddPath(vPath);

    // ���Զ�����
    m_pFlowCtrl->ReadTree(m_strDllXmlPath, m_vTestElement);   

    m_hRoot = m_treeCtrl.InsertItem(_T("��������"));
    CreateTree(m_vTestElement);
    m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoBinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
            m_treeCtrl.InsertItem(strName, treeEle);
        }

        m_treeCtrl.Expand(treeEle, TVE_EXPAND);
    }

    return true;
}


void CAutoBinDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here

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
    
    HTREEITEM hTestElement = m_treeCtrl.GetChildItem(m_hRoot);
    while (hTestElement)
    {
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
    CreateTestPath();

    m_pFlowCtrl->StartTest(m_strPathIn, m_vTestPath);
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
           CreateTestAtom(itr, name);
        }

        if (itr->id > id)
        {
            if (itr->desc != desc)
            {
                m_vTestPath.push_back(*itr);
            }
            else
            {
                CreateTestAtom(itr, name);
                break;
            }   
        }
    }
}

void CAutoBinDlg::CreateTestAtom( vector<TestElement>::iterator itr, string& strName )
{
    TestElement testEle;
    testEle.desc = itr->desc;
    testEle.id = itr->id;

    vector<TestAtom>::iterator itrAtom = itr->vTestAtom.begin();
    for (; itrAtom != itr->vTestAtom.end(); ++itrAtom)
    {
        if (itrAtom->name == strName)
        {
            testEle.vTestAtom.push_back(*itrAtom);
        }
    }

    m_vTestPath.push_back(testEle);
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

    m_hRoot = m_treeCtrl.InsertItem(_T("��������"));

    m_pFlowCtrl->ReadTree(m_strExeXmlPath, m_vTestElement);
    CreateTree(m_vTestElement);
    m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
}

void CAutoBinDlg::OnBnClickedRadio1()
{
    m_treeCtrl.DeleteAllItems();

    m_hRoot = m_treeCtrl.InsertItem(_T("��������"));

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

    CDialog::OnTimer(nIDEvent);
}
