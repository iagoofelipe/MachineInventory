#include "app.h"
#include "appmodel.h"
#include "appcontroller.h"

bool inventory::MachineInventoryApp::OnInit()
{
    AppController::GetInstance().Initialize();
    return true;
}

int inventory::MachineInventoryApp::OnExit()
{
    AppModel::GetInstance()->Cleanup();
    return wxApp::OnExit();
}