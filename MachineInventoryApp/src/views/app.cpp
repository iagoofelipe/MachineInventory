#include "views/app.h"
#include "models/appmodel.h"
#include "controllers/appcontroller.h"

bool inventory::MachineInventoryApp::OnInit()
{
    AppController::GetInstance().Initialize();
    return true;
}

int inventory::MachineInventoryApp::OnExit()
{
    AppModel::GetInstance().Cleanup();
    return wxApp::OnExit();
}