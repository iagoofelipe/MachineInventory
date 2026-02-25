#ifndef APP_H
#define APP_H

#include <wx/wx.h>

namespace inventory
{

class MachineInventoryApp : public wxApp
{
public:
    virtual bool OnInit();
};

}

#endif