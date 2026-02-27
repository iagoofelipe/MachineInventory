#ifndef APPMODEL_H
#define APPMODEL_H

#include <sysinfo/server.h>
#include <wx/wx.h>

wxDECLARE_EVENT(EVT_APPMODEL_MESSAGE, wxCommandEvent);
wxDECLARE_EVENT(EVT_APPMODEL_INIT, wxCommandEvent);
wxDECLARE_EVENT(EVT_APPMODEL_LOGIN, wxCommandEvent);

namespace inventory
{
    class AppModel : public wxEvtHandler
    {
        AppModel();

    public:
        // Singleton
        AppModel(const AppModel&) = delete;
        void operator=(const AppModel&) = delete;
        static AppModel& GetInstance();
        
        void Initialize();
        void Cleanup() {}
        
        const wxString& GetLastError() const { return lastError; }
        sysinfo::user* GetLoggedUser() { return isLogged? &loggedUser : nullptr; }
        sysinfo::machine* GetMachine() { return &machine; }
        
        void Auth(const wxString& cpf, const wxString& password);
        void Logout();
        
    private:
        wxString lastError;
        sysinfo::ServerAPI server;
        sysinfo::machine machine;
        sysinfo::user loggedUser;
        bool isLogged;

        void queueMessage(const wxString& msg);
        void queueInit(bool success);
    };
}

#endif