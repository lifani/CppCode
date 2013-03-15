#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <cstdio>
#include <iostream>
#include <string>

#include <Windows.h>
#include <process.h>

#include <vector>
#include <list>
#include <map>

#ifdef DLL_EXPORTS
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspec(dllimport)
#endif

using namespace std;

// 测试对象树叶子节点
typedef struct _TestAtom
{
    _TestAtom() : id(0), nResult(0), name(""), p(NULL)
    {
    }

    int id;
    int nResult;
    void* p;
    string name;
}TestAtom;

// 测试对象树阶段节点
typedef struct _TestElement
{
    _TestElement () :bRunning(false), id(0), desc(""), p(NULL)
    {

    }

    bool bRunning;
    int id;
    void* p;
    string desc;
    vector<TestAtom> vTestAtom;
}TestElement;

// 测试任务tag
typedef struct _TestTag
{
    _TestTag () : strTestPath(""), nTestResult(0)
    {

    }

    string strTestPath;
    int nTestResult;
}TestTag;

// 线程tag
typedef struct _ThreadTag
{
    _ThreadTag () : id(0), name(""), time(""), nRun(0)
    {

    }

    int id;
    string name;
    string time;

    vector<TestTag> vTestTag;
    int nRun;
}ThreadTag;


#define T_ID    "id"
#define T_NAME  "name"
#define T_DESC  "desc"

#define N_ROOT  "root"
#define N_TAGS  "tags"
#define N_TAG   "tag"

#define N_ARGS  "args"
#define N_ARG   "arg"

#define N_PATHS "paths"
#define N_PATH  "path"

#define MS_OK    0
#define MS_ERR   1

#define RESULT_PATH "result"
#define SEPERATOR "\\"

#define DLL_XML         "dll.xml"
#define EXE_XML         "exe.xml"
#define PATH_XML        "path.xml"
#define EXECUTE_XML     "execute.xml"

#endif
