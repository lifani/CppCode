#include <iostream>
#include "../flowCtrl/FlowCtrl.h"
#include "../TestDllBox/TestDllBox.h"
#include "../common/DllInterface.h"

using namespace std;

int main(int argc, char* argv[])
{
   /* CFlowCtrl* pFlowCtrl = CFlowCtrl::Instance();

    pFlowCtrl->Init();

    vector<TestElement> vTestElement;

    pFlowCtrl->ReadTree("E:\\Code\\AutoTest\\dll.xml", vTestElement);

    pFlowCtrl->StartTest("E:\\Code\\AutoTest", vTestElement);*/

    //CTestDllBox* pTestDllBox = CTestDllBox::Instance();

    HMODULE hdl = LoadLibraryA("./TestDll.dll");
    
    func fun = (func)GetProcAddress(hdl, "CreateInstance");
    if (fun)
    {
        CDllInterface* pDll = fun();
        pDll->StartTest("hello world!", "i love my baby!");
    }

    return 0;
}