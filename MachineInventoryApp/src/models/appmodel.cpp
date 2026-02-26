#include "appmodel.h"

inventory::AppModel& inventory::AppModel::getInstance()
{
    static AppModel instance;
    return instance;
}

inventory::AppModel::AppModel()
{}

bool inventory::AppModel::initialize()
{
    return true;
}