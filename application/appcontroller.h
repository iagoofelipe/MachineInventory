#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "appview.h"
#include "appmodel.h"
#include "structs.h"

#include <wx/wx.h>

namespace inventory
{
    class AppController : public wxEvtHandler
    {

    public:
        // Singleton
        AppController(const AppController&) = delete;
        void operator=(const AppController&) = delete;
        static AppController& GetInstance();

        void Initialize();

    private:
        AppModel* model;
        AppView* view;

        AppController();

        // Eventos
        void On_AppView_Logout(wxCommandEvent& event);

        void On_AuthForm_Auth(wxCommandEvent& event);
        void On_AuthForm_CreateAccount(wxCommandEvent& event);
        void On_AuthForm_Machine(wxCommandEvent& event);
        
        void On_AppModel_Message(wxCommandEvent& event);
        void On_AppModel_Init(wxCommandEvent& event);
        void On_AppModel_Login(wxCommandEvent& event);
    };
}

#endif