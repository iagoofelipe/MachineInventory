#include "appcontroller.h"

inventory::AppController& inventory::AppController::getInstance()
{
    static AppController instance;
    return instance;
}

inventory::AppController::AppController()
    : model(AppModel::getInstance())
    , view(AppView::getInstance())
{}

bool inventory::AppController::initialize()
{
    return view.initialize();
}