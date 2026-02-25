#ifndef APPMODEL_H
#define APPMODEL_H

#include <wx/wx.h>

namespace inventory
{

class AppModel
{

public:
    // Singleton
    AppModel(const AppModel&) = delete;
    void operator=(const AppModel&) = delete;
    static AppModel& getInstance();
    
    bool initialize();
    
private:
    AppModel();

};

}

#endif