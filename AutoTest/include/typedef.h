#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <cstdio>
#include <iostream>
#include <string>

#include <vector>
#include <list>
#include <map>

#ifdef AUTOTEST_EXPORTS
#define AUTOTEST_API _declspec(dllexport)
#else
#define AUTOTEST_API _declspec(dllimport)
#endif

using namespace std;

// ���Զ�����Ҷ�ӽڵ�
typedef struct _TestAtom
{
    int id;
    string name;
}TestAtom;

// ���Զ������׶νڵ�
typedef struct _TestElement
{
    int id;
    string desc;
    vector<TestAtom> vTestAtom;
}TestElement;

// ��������tag
typedef struct _TestTag
{
    string strTestPath;
    int nTestResult;
}TestTag;

// �߳�tag
typedef struct _ThreadTag
{
    string name;
    vector<TestTag> vTestTag;
    bool bRunning;
};

#define T_ID    "id"
#define T_NAME  "name"
#define T_DESC  "desc"

#define N_ROOT  "root"
#define N_TAGS  "tags"
#define N_TAG   "tag"

#define N_PATHS "paths"
#define N_PATH  "path"

#endif
