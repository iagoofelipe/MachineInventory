#ifndef ID_H
#define ID_H

#include <wx/wx.h>

namespace inventory
{
    enum ID
    {
        // App View
        ID_APP_VIEW_MENU_LOGOUT = wxID_HIGHEST + 1,
        ID_APP_VIEW_TIMER_STATUSBAR,

		// Auth Form
        ID_AUTH_FORM,

		// Loading Form
        ID_LOADING_FORM,

		// Home Form
        ID_HOME_FORM,

        // Machine Form
        ID_MACHINE_FORM,
        ID_MACHINE_FORM_BTN_SYNC,

		// Sync Form
        ID_SYNC_FORM,
        ID_SYNC_FORM_CHECKBOX,
        ID_SYNC_FORM_BTN_BACK,
		ID_SYNC_FORM_BTN_CONTINUE,
    };
}

#endif
