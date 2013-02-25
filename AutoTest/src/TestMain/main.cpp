#include <iostream>
#include "../flowCtrl/FlowCtrl.h"

using namespace std;

int main(int argc, char* argv[])
{
    CFlowCtrl* pFlowCtrl = CFlowCtrl::Instance();

    pFlowCtrl->Init();

    vector<TestElement> vTestElement;

    pFlowCtrl->ReadTree("E:\\Code\\AutoTest\\dll.xml", vTestElement);

    pFlowCtrl->StartTest("E:\\Code\\AutoTest", vTestElement);

    return 0;
}