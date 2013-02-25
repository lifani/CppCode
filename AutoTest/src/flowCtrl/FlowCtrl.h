// FlowCtrl.h
// author   :       Li Fa Ni
// Date     :       2013-02-21
// Ver      :       1.0

#ifndef _FLOWCTRL_H_
#define _FLOWCTRL_H_

#include "../../include/typedef.h"
#include "../xml/xml.h"
#include "../common/BaseThread.h"

class DLL_API CFlowCtrl : public CBaseThread
{
public:

    static CFlowCtrl* Instance()
    {
        if (NULL == m_pFlowCtrl)
        {
            m_pFlowCtrl = new CFlowCtrl;
        }

        return m_pFlowCtrl;
    }

    virtual ~CFlowCtrl(void);

    bool Init();

    bool ReadTree(const string& strIn, vector<TestElement>& vTestElement);

    bool SaveTree(const string& strIn, vector<TestElement>& vTestElement);

    bool ReadPath(const string& strIn, vector<string>& vPath);

    bool SavePath(const string& strIn, vector<string>& vPath);

    bool StartTest(const string& strIn, vector<TestElement>& vTestElement, bool isEnforce = false);

    bool EndTest();

private:

    CFlowCtrl(void);

    void Run();

    bool EndThreadNormal(DWORD ulTimes = 10);

    int CreateSaveDirectory(const string& strIn);

    bool CreateTestPath(vector<TestElement>& vTestElement);

    void PushBackVTestTag(vector<TestTag>& vTestTag);

    bool NeedExecute(TestTag& testTag);

    string GetAppPath();
private:

    static CFlowCtrl* m_pFlowCtrl;

    CXml m_CXml;

    vector<ThreadTag>* m_pVThreadTag;

    vector<string>* m_pTrack;

    // 软件运行路径
    string m_strAppPath;
    // 输入文件路径
    string m_strPathIn;
    // 结果保存路径
    string m_strResultPath;
};

#endif
