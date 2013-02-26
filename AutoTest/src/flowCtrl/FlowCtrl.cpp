// FlowCtrl.cpp
// author   :       Li Fa Ni
// Date     :       2013-02-21
// Ver      :       1.0

#include "../TestDllBox/TestDllBox.h"
#include "FlowCtrl.h"
#include "../ToolBox/toolBox.h"

CFlowCtrl* CFlowCtrl::m_pFlowCtrl = NULL;

CFlowCtrl* CFlowCtrl::Instance()
{
    if (NULL == m_pFlowCtrl)
    {
        m_pFlowCtrl = new CFlowCtrl;
    }

    return m_pFlowCtrl;
}

CFlowCtrl::CFlowCtrl(void): m_pVThreadTag(NULL), m_pTrack(NULL)
{
    
}

CFlowCtrl::~CFlowCtrl(void)
{
    Destory();
}

bool CFlowCtrl::Init()
{
    m_strAppPath = GetAppPath();

    m_pVThreadTag   = new vector<ThreadTag>;
    m_pTrack        = new vector<ThreadTag>;
    m_pTestDllBox   = CTestDllBox::Instance();

    if (NULL == m_pVThreadTag || NULL == m_pTrack || NULL == m_pTestDllBox)
    {
        Destory();
        return false;
    }

    return true;
}

bool CFlowCtrl::ReadTree(const string& strIn, vector<TestElement>& vTestElement)
{
    return m_CXml.ReadTreeXml(strIn, vTestElement);
}

bool CFlowCtrl::SaveTree(const string& strIn, vector<TestElement>& vTestElement)
{
    return m_CXml.WriteTreeXml(strIn, vTestElement);
}

bool CFlowCtrl::ReadPath(const string& strIn, vector<string>& vPath)
{
    return m_CXml.ReadPathXml(strIn, vPath);
}

bool CFlowCtrl::SavePath(const string& strIn, vector<string>& vPath)
{
    return m_CXml.WritePathXml(strIn, vPath);
}

void CFlowCtrl::Run()
{

}

bool CFlowCtrl::EndThreadNormal( DWORD ulTimes /*= 10*/ )
{
    return true;
}

bool CFlowCtrl::StartTest( const string& strIn, vector<TestElement>& vTestElement, bool isEnforce )
{
    if (strIn.empty())
    {
        return false;
    }

    string strName = strIn;

    // '\' | '/' => $, ':' = > #
    Replace(strName, '\\', '$');
    Replace(strName, '/', '$');
    Replace(strName, ':', '#');

    // There is result save directory?
    m_strResultPath = m_strAppPath + SEPERATOR + RESULT_PATH + SEPERATOR + strName;
    if (!CheckFileExist(m_strResultPath))
    {
        if (MS_OK != CreateSaveDirectory(m_strResultPath))
        {
            return false;
        }
    }

    // 生成测试执行路径
    if (!CreateTestPath(vTestElement))
    {
        return false;
    }

    // 使用完毕后清理
    m_pTrack->clear();

    // 执行测试
    m_pTestDllBox->StartTest(m_pVThreadTag, m_strAppPath, m_strPathIn, m_strResultPath);

    // 启动监控线程

    return true;
}

int CFlowCtrl::CreateSaveDirectory( const string& strIn )
{
    return CreateDirectorys(m_strResultPath);
}

bool CFlowCtrl::CreateTestPath( vector<TestElement>& vTestElement )
{
    // Clear
    m_pVThreadTag->clear();
    m_pTrack->clear();

    vector<ThreadTag> vThreadTag;

    vector<TestElement>::iterator itrEle = vTestElement.begin();
    for (; itrEle != vTestElement.end(); ++itrEle)
    {
        vThreadTag.clear();

        vector<TestAtom>::iterator itrAtom = itrEle->vTestAtom.begin();
        for (; itrAtom != itrEle->vTestAtom.end(); ++itrAtom)
        {
            PushBackVTestTag(*itrAtom, vThreadTag);
        }

        m_pTrack->clear();

        m_pTrack->insert(m_pTrack->end(), vThreadTag.begin(), vThreadTag.end());
        m_pVThreadTag->insert(m_pVThreadTag->end(), vThreadTag.begin(), vThreadTag.end());
    }

    return true;
}

void CFlowCtrl::PushBackVTestTag( TestAtom& tAtom, vector<ThreadTag>& vThreadTag )
{
    ThreadTag threadTag;
    
    threadTag.id        = tAtom.id;
    threadTag.name      = tAtom.name;
    threadTag.bRunning  = false;

    string strDllPath = m_strAppPath + SEPERATOR + tAtom.name;
    GetCreationTime(strDllPath, threadTag.time);

    if (m_pTrack->empty())
    {
        TestTag tTag;
        tTag.nTestResult = 0;
        tTag.strTestPath = "$" + tAtom.name;

        threadTag.vTestTag.push_back(tTag);
    }
    else
    {
        vector<ThreadTag>::iterator itr = m_pTrack->begin();
        for (; itr != m_pTrack->end(); ++itr)
        {
            vector<TestTag>::iterator it = itr->vTestTag.begin();
            for (; it != itr->vTestTag.end(); ++it)
            {
                TestTag tTag;
                tTag.nTestResult = 0;
                tTag.strTestPath = it->strTestPath + "$" + tAtom.name;

                threadTag.vTestTag.push_back(tTag);
            }
        }
    }

    vThreadTag.push_back(threadTag);
}

string CFlowCtrl::GetAppPath()
{
    char szPath[MAX_PATH] = {0};

    GetModuleFileNameA(NULL, szPath, MAX_PATH);

    for (unsigned i = MAX_PATH - 1; i >= 0; --i)
    {
        if ('\\' == szPath[i] || '/' == szPath[i])
        {
            szPath[i] = '\0';
            break;
        }
    }

    return string(szPath);
}

void CFlowCtrl::Destory()
{
    if (NULL != m_pVThreadTag)
    {
        delete m_pVThreadTag;
        m_pVThreadTag = NULL;
    }

    if (NULL != m_pTrack)
    {
        delete m_pTrack;
        m_pTrack = NULL;
    }

    if (NULL != m_pTestDllBox)
    {
        delete m_pTestDllBox;
        m_pTestDllBox = NULL;
    }
}
