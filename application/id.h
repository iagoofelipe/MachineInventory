#ifndef ID_H
#define ID_H

#include <wx/wx.h>

namespace inventory
{
    enum ID
    {
        ID_AUTH_FORM = wxID_HIGHEST + 1,
        ID_LOADING_FORM,
        ID_SYNC_FORM,
        ID_MACHINE_FORM,
        ID_HOME_FORM,
        ID_MENU_LOGOUT,
        ID_TIMER_STATUSBAR,
    };
} // namespace inventory

#endif
