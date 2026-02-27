#ifndef APPVIEW_H
#define APPVIEW_H

#include "views/forms/authform.h"
#include "views/forms/loadingform.h"
#include "views/forms/syncform.h"
#include "views/forms/machineform.h"
#include "views/forms/homeform.h"
#include "models/appmodel.h"

#include <wx/wx.h>

wxDECLARE_EVENT(EVT_APPVIEW_UI, wxCommandEvent);
wxDECLARE_EVENT(EVT_APPVIEW_LOGOUT, wxCommandEvent);

namespace inventory
{
    class AppView : public wxEvtHandler
    {
        AppView();

    public:
        enum UI
        {
            NONE = 0,
            LOADING = 1 << 1,
            AUTH = 1 << 2,
            SYNC = 1 << 3,
            MACHINE = 1 << 4,
            HOME = SYNC | MACHINE,
        };

        // Singleton
        AppView(const AppView&) = delete;
        void operator=(const AppView&) = delete;
        static AppView& GetInstance();

        void Initialize();
        void* SetUI(UI ui);
        AuthForm* GetAuthForm() const { return authForm; }
        LoadingForm* GetLoadingForm() const { return loadingForm; }
        SyncForm* GetSyncForm() const { return syncForm; }
        MachineForm* GetMachineForm() const { return machineForm; }
        HomeForm* GetHomeForm() const { return homeForm; }
        IForm* GetCurrentForm() const { return form; }

    private:
        const int ID_MENU_LOGOUT;
        AppModel& model;
        UI currentUI;
        void* currentWidget;
        wxBoxSizer* sizer;
        wxFrame* window;
        wxMenuBar* menuBar;

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