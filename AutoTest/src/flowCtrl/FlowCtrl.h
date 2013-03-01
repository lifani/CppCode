// FlowCtrl.h
// author   :       Li Fa Ni
// Date     :       2013-02-21
// Ver      :       1.0

#ifndef _FLOWCTRL_H_
#define _FLOWCTRL_H_

#include "../../include/typedef.h"
#include "../xml/xml.h"
#include "../TestDllBox/TestDllBox.h"

class DLL_API CFlowCtrl
{
public:

    static CFlowCtrl* Instance();

    virtual ~CFlowCtrl(void);

    bool Init();

    bool ReadTree(const string& strIn, vector<TestElement>& vTestElement);

    bool SaveTree(const string& strIn, vector<TestElement>& vTestElement);

    bool ReadPath(const string& strIn, vector<string>& vPath);

    bool SavePath(const string& strIn, vector<string>& vPath);

    bool StartTest(const string& strIn, vector<TestElement>& vTestElement, bool isEnforce = false);

    bool EndTest();

    void GetProgress(vector<TestElement>& vTestElement);

    string GetAppPath();

private:

    CFlowCtrl(void);

    bool EndThreadNormal(DWORD ulTimes = 10);

    int CreateSaveDirectory(const string& strIn);

    bool CreateTestPath(vector<TestElement>& vTestElement);

    void PushBackVTestTag(TestAtom& tAtom, vector<ThreadTag>& vThreadTag);

    int GetProgressImp(const int id, const string& strName);

    void ExecutionJudge(vector<ThreadTag>& vThreadTag);

    void Destory();
private:

    static CFlowCtrl* m_pFlowCtrl;

    CTestDllBox* m_pTestDllBox;

    CXml m_CXml;

    vector<ThreadTag>* m_pVThreadTag;

    vector<ThreadTag>* m_pTrack;

    // 软件运行路径
    string m_strAppPath;
    // 输入文件路径
    string m_strPathIn;
    // 结果保存路径
    string m_strResultPath;
};

#endif
