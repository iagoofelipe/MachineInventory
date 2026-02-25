#include "app.h"
#include "controllers/appcontroller.h"

bool inventory::MachineInventoryApp::OnInit()
{
    AppController& controller = AppController::getInstance();
    return controller.initialize();
}