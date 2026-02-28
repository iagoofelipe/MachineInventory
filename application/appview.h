#ifndef APPVIEW_H
#define APPVIEW_H

#include "authform.h"
#include "loadingform.h"
#include "syncform.h"
#include "machineform.h"
#include "homeform.h"
#include "appmodel.h"

#include <wx/wx.h>

wxDECLARE_EVENT(EVT_APPVIEW_LOGOUT, wxCommandEvent);

namespace inventory
{
    class AppView : public wxFrame
    {
    public:
        AppView();

        // enum UI
        // {
        //     NONE = 0,
        //     UI_LOADING = 1 << 1,
        //     UI_AUTH = 1 << 2,
        //     UI_SYNC = 1 << 3,
        //     UI_MACHINE = 1 << 4,
        //     UI_HOME = UI_SYNC | UI_MACHINE,
        // };

        void Initialize();
        void* SetUI(int id);
        void ShowMessage(const wxString& msg, int timeout = 0); // timeout deve ser maior out igual a 1000

        AuthForm* GetAuthForm() const { return authForm; }
        LoadingForm* GetLoadingForm() const { return loadingForm; }
        SyncForm* GetSyncForm() const { return syncForm; }
        MachineForm* GetMachineForm() const { return machineForm; }
        HomeForm* GetHomeForm() const { return homeForm; }
        IForm* GetCurrentForm() const { return form; }

    private:
        int currentUI;
        void* currentWidget;
        wxBoxSizer* sizer;
        wxMenuBar* menuBar;
        wxTimer timerStatusbar;

        IForm* form;
        LoadingForm* loadingForm;
        SyncForm* syncForm;
        AuthForm* authForm;
        MachineForm* machineForm;
        HomeForm* homeForm;

        void setupUI();
    };
}

#endif