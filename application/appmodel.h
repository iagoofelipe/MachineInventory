#ifndef APPMODEL_H
#define APPMODEL_H

#include <sysinfo/server.h>
#include <wx/wx.h>
#include <wx/datetime.h>

wxDECLARE_EVENT(EVT_APPMODEL_MESSAGE, wxCommandEvent);
wxDECLARE_EVENT(EVT_APPMODEL_INIT, wxCommandEvent);
wxDECLARE_EVENT(EVT_APPMODEL_LOGIN, wxCommandEvent);
wxDECLARE_EVENT(EVT_APPMODEL_MACHINE, wxCommandEvent);

namespace inventory
{
    struct machine_extraction
    {
        sysinfo::machine data;
        wxString title;
        wxDateTime datetime;
    };

    class AppModel : public wxEvtHandler
    {
        AppModel();

    public:
        // Singleton
        AppModel(const AppModel&) = delete;
        void operator=(const AppModel&) = delete;
        static AppModel* GetInstance();
        
        void Initialize();
        void Cleanup() {}
        
        const wxString& GetLastError() const { return lastError; }
        const sysinfo::user* GetLoggedUser() { return isLogged? &loggedUser : nullptr; }
        const sysinfo::machine* GetMachine() { return &extraction.data; }
        const machine_extraction* GetExtraction() { return &extraction; }
        void SetExtractionTitle(const wxString& title) { extraction.title = title; }
        
        void Auth(const wxString& cpf, const wxString& password);
        void Logout();
        void UpdateExtraction();
        
    private:
        wxString lastError;
        sysinfo::ServerAPI server;
        machine_extraction extraction;
        sysinfo::user loggedUser;
        bool isLogged;

        void queueMessage(const wxString& msg);
        void queueInit(bool success);
        bool extractMachine();
    };
}

#endif