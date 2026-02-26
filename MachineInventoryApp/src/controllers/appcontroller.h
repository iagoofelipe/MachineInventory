#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "views/appview.h"
#include "models/appmodel.h"

#include <wx/wx.h>

namespace inventory
{

class AppController
{

public:
    // Singleton
    AppController(const AppController&) = delete;
    void operator=(const AppController&) = delete;
    static AppController& getInstance();

    bool initialize();

private:
    AppModel& model;
    AppView& view;

    AppController();

};

}

#endif