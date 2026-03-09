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
wxDECLARE_EVENT(EVT_APPMODEL_SERVER_CONN, wxCommandEvent);   // Conexão estabelecida com o servidor
wxDECLARE_EVENT(EVT_APPMODEL_SERVER_LOST, wxCommandEvent);   // Conexão perdida com o servidor

namespace inventory
{
    struct machine_extraction
    {
        sysinfo::machine data;
        wxString title;
        wxDateTime datetime;
    };

    struct user
    {
        wxString id;
        wxString cpf;
        wxString name;
        int rules = 0;

        user() {}

        user(const sysinfo::user& u)
            : user(&u)
        {}

        user(const sysinfo::user* u)
            : id(wxString::FromUTF8(u->id))
            , cpf(wxString::FromUTF8(u->cpf))
            , name(wxString::FromUTF8(u->name))
            , rules(u->rules)
        {}
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
        void Cleanup();
        
        const wxString& GetLastError() const { return lastError; }
		const sysinfo::ServerAPI* GetServer() { return &server; }
        const user* GetLoggedUser() { return isLogged? &loggedUser : nullptr; }
        const user* GetOwner() { return isLogged? &owner : nullptr; }
        const sysinfo::machine* GetMachine() { return &extraction.data; }
        const machine_extraction* GetExtraction() { return &extraction; }
        
		// Machine Extraction Methods
        void SetExtractionTitle(const wxString& title) { extraction.title = title; }
        void UpdateExtraction();
		void SendExtractionToServer();
        const wxString& GetPreviousExtractionTitle() const { return prevExtractionTitle; }
        
        // User Methods
        void Auth(const wxString& cpf, const wxString& password);
        void Logout();
        bool HasUserRule(sysinfo::USER_RULE rule) const { return isLogged && (loggedUser.rules & rule); }
		void QueryOwner(const wxString& cpf);
		void CreateUser(const wxString& cpf, const wxString& name, const wxString& password);
        void SetLoggedUserAsOwner();

    private:
        wxString lastError;
        wxString prevExtractionTitle;
        sysinfo::ServerAPI server;
        machine_extraction extraction;
        user
            loggedUser,
            owner;
        bool isLogged;
        FILE* logFile = nullptr;

        void queueMessage(const wxString& msg);
        void queueError(const wxString& msg);
        void queueInit(bool success);
        bool extractMachine();
    };
}

#endif