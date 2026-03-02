#ifndef APPMODEL_H
#define APPMODEL_H

#include <sysinfo/server.h>
#include <wx/wx.h>
#include <wx/datetime.h>

wxDECLARE_EVENT(EVT_APPMODEL_MESSAGE, wxCommandEvent);       // Nova mensagem referente a um processo
wxDECLARE_EVENT(EVT_APPMODEL_INIT, wxCommandEvent);          // Finalização do processo de inicialização
wxDECLARE_EVENT(EVT_APPMODEL_LOGIN, wxCommandEvent);         // Finalização do processo de autenticação
wxDECLARE_EVENT(EVT_APPMODEL_MACHINE, wxCommandEvent);       // Finalização do processo de extração de dados da máquina
wxDECLARE_EVENT(EVT_APPMODEL_QUERY_OWNER, wxCommandEvent);   // Finalização do processo de consulta de proprietário
wxDECLARE_EVENT(EVT_APPMODEL_CREATE_USER, wxCommandEvent);   // Finalização do processo de criação de usuário
wxDECLARE_EVENT(EVT_APPMODEL_SERVER, wxCommandEvent);        // Finalização do processo de envio dos dados da máquina para o servidor

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
        
		// Machine Extraction Methods
        void SetExtractionTitle(const wxString& title) { extraction.title = title; }
        void UpdateExtraction();
		void SendExtractionToServer();
        
        // User Methods
        void Auth(const wxString& cpf, const wxString& password);
        void Logout();
        bool HasUserRule(sysinfo::USER_RULE rule) const { return isLogged && (loggedUser.rules & rule); }
		void QueryOwner(const wxString& cpf);
		void CreateUser(const wxString& cpf, const wxString& name, const wxString& password);

    private:
        wxString lastError;
        sysinfo::ServerAPI server;
        machine_extraction extraction;
        sysinfo::user
            loggedUser,
            owner;
        bool isLogged;

        void queueMessage(const wxString& msg);
        void queueInit(bool success);
        bool extractMachine();
    };
}

#endif