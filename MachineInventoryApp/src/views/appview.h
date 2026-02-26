#ifndef APPVIEW_H
#define APPVIEW_H

#include "models/appmodel.h"

#include <wx/wx.h>

namespace inventory
{

class AppView
{

public:
    // Singleton
    AppView(const AppView&) = delete;
    void operator=(const AppView&) = delete;
    static AppView& getInstance();

    bool initialize();

private:
    AppModel& model;
    wxFrame* window;

    AppView();

};

}

#endif