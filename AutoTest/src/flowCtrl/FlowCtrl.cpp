// FlowCtrl.cpp
// author   :       Li Fa Ni
// Date     :       2013-02-21
// Ver      :       1.0

#include "FlowCtrl.h"
#include "../ToolBox/toolBox.h"

CFlowCtrl* CFlowCtrl::m_pFlowCtrl = NULL;

CFlowCtrl::CFlowCtrl(void)
{
    m_pVThreadTag = new vector<ThreadTag>;
    m_pTrack = new vector<string>;
}

CFlowCtrl::~CFlowCtrl(void)
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
}

bool CFlowCtrl::Init()
{
    m_strAppPath = GetAppPath();

    if (NULL == m_pVThreadTag)
    {
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

    // 执行测试

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

    vector<TestTag> vTestTag;

    vector<TestElement>::iterator itrEle = vTestElement.begin();
    for (; itrEle != vTestElement.end(); ++itrEle)
    {
        vTestTag.clear();
    
        vector<TestAtom>::iterator itrAtom = itrEle->vTestAtom.begin();
        for (; itrAtom != itrEle->vTestAtom.end(); ++itrAtom)
        {
            TestTag testTag;
            
            testTag.nTestResult = 0;
            testTag.strTestPath = "$" + itrAtom->name;

            vTestTag.push_back(testTag);
        }

        PushBackVTestTag(vTestTag);
    }

    return true;
}

void CFlowCtrl::PushBackVTestTag( vector<TestTag>& vTestTag )
{
    ThreadTag threadTag;

    if (m_pVThreadTag->empty())
    {
        threadTag.vTestTag = vTestTag;
    }
    else
    {
        ThreadTag& tTag =  m_pVThreadTag->back();

        vector<TestTag>::iterator itrTag = vTestTag.begin();
        for (; itrTag != vTestTag.end(); ++itrTag)
        {
            vector<TestTag>::iterator itr = tTag.vTestTag.begin();
            for (; itr != tTag.vTestTag.end(); ++itr)
            {
                TestTag tag;

                tag.nTestResult = 0;
                tag.strTestPath = itr->strTestPath + itrTag->strTestPath;

                threadTag.vTestTag.push_back(tag);
            }
        }
    }

    threadTag.bRunning = false;
    m_pVThreadTag->push_back(threadTag);
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
