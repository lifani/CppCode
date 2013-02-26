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
    int id;
    string name;
}TestAtom;

// 测试对象树阶段节点
typedef struct _TestElement
{
    int id;
    string desc;
    vector<TestAtom> vTestAtom;
}TestElement;

// 测试任务tag
typedef struct _TestTag
{
    string strTestPath;
    int nTestResult;
}TestTag;

// 线程tag
typedef struct _ThreadTag
{
    int id;
    string name;
    string time;

    vector<TestTag> vTestTag;
    bool bRunning;
}ThreadTag;

#define T_ID    "id"
#define T_NAME  "name"
#define T_DESC  "desc"

#define N_ROOT  "root"
#define N_TAGS  "tags"
#define N_TAG   "tag"

#define N_PATHS "paths"
#define N_PATH  "path"

#define MS_OK    0
#define MS_ERR   1

#define RESULT_PATH "result"
#define SEPERATOR "\\"

#endif
