#include "appmodel.h"

#include <thread>
#include <wx/log.h> 

wxDEFINE_EVENT(EVT_APPMODEL_MESSAGE, wxCommandEvent);       // Nova mensagem referente a um processo
wxDEFINE_EVENT(EVT_APPMODEL_INIT, wxCommandEvent);          // Finalização do processo de inicialização
wxDEFINE_EVENT(EVT_APPMODEL_LOGIN, wxCommandEvent);         // Finalização do processo de autenticação
wxDEFINE_EVENT(EVT_APPMODEL_MACHINE, wxCommandEvent);       // Finalização do processo de extração de dados da máquina
wxDEFINE_EVENT(EVT_APPMODEL_QUERY_OWNER, wxCommandEvent);   // Finalização do processo de consulta de proprietário
wxDEFINE_EVENT(EVT_APPMODEL_CREATE_USER, wxCommandEvent);   // Finalização do processo de criação de usuário
wxDEFINE_EVENT(EVT_APPMODEL_SERVER, wxCommandEvent);        // Finalização do processo de envio dos dados da máquina para o servidor

inventory::AppModel::AppModel()
    : wxEvtHandler()
    , isLogged(false)
{}

inventory::AppModel* inventory::AppModel::GetInstance()
{
    static AppModel instance;
    return &instance;
}

void inventory::AppModel::Initialize()
{
    std::thread worker([this] {
        if (!extractMachine()) {
            queueInit(0);
            return;
        }

        queueMessage(wxString::FromUTF8("inicialiando conexão com servidor..."));
        if (!server.Initialize()) {
            queueMessage(wxString::FromUTF8(server.GetLastError()));
            queueInit(0);
            return;
        }

        queueMessage(wxString::FromUTF8("coletando dados do usuário..."));
        isLogged = false;
        if (server.HasToken()) {
            isLogged = server.GetUser(&loggedUser);
        }

        queueInit(1);
    });

    worker.detach();
}

void inventory::AppModel::Auth(const wxString& cpf, const wxString& password)
{
    std::thread worker([this, cpf, password] {

        if (
            !server.Auth(cpf.ToStdString(), password.ToStdString()) ||
            !server.GetUser(&loggedUser)
        ) {
            isLogged = false;
            queueMessage(wxString::FromUTF8(server.GetLastError()));
        }
        else {
            isLogged = true;
            owner = loggedUser;
        }

        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_LOGIN);
        evt->SetInt(isLogged);
        QueueEvent(evt);
    });

    worker.detach();
}

void inventory::AppModel::Logout()
{
    if (!isLogged)
        return;
    
    server.ClearToken();
    isLogged = false;
}

void inventory::AppModel::UpdateExtraction()
{
    std::thread worker([this] {
        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_MACHINE);
        int success = extractMachine();
        
        evt->SetInt(success);
        evt->SetClientData(success? (void*)GetExtraction() : nullptr);
        QueueEvent(evt);
    });

    worker.detach();
}

void inventory::AppModel::SendExtractionToServer()
{
    std::thread worker([this] {
        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_SERVER);
		queueMessage(wxString::FromUTF8("enviando dados para o servidor..."));
        int success = server.UploadMachine(&extraction.data, owner.cpf.c_str(), extraction.title.c_str());
        
        if (!success)
            queueMessage(wxString::Format("erro ao enviar os dados: %s", server.GetLastError()));
        else
            queueMessage(wxString::FromUTF8("dados enviados com êxito"));

        evt->SetInt(success);
        QueueEvent(evt);
    });

	worker.detach();
}

void inventory::AppModel::QueryOwner(const wxString& cpf)
{
    std::thread worker([this, cpf] {
        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_QUERY_OWNER);

        if (server.GetUser(&owner, cpf.ToStdString())) {
            evt->SetInt(1);
            evt->SetClientData(&owner);
        }
        else {
            evt->SetInt(0);
            evt->SetClientData(nullptr);
            queueMessage(wxString::FromUTF8(server.GetLastError()));
        }

        QueueEvent(evt);
    });

	worker.detach();
}

void inventory::AppModel::CreateUser(const wxString& cpf, const wxString& name, const wxString& password)
{
    std::thread worker([this, cpf, name, password] {
        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_CREATE_USER);
        int success;

        if (!(success = server.CreateNewUser(cpf.ToStdString(), name.ToStdString(), password.ToStdString())))
            queueMessage(wxString::FromUTF8(server.GetLastError()));

        evt->SetInt(success);
		QueueEvent(evt);
    });

    worker.detach();
}

void inventory::AppModel::queueMessage(const wxString& msg)
{
    wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_MESSAGE);
    evt->SetString(msg);
    QueueEvent(evt);
}

void inventory::AppModel::queueInit(bool success)
{
    wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_INIT);
    evt->SetInt(success);
    QueueEvent(evt);
}

bool inventory::AppModel::extractMachine()
{
    queueMessage("inicialiando componentes do sistema...");
    if (!sysinfo::Init()) {
        queueMessage(sysinfo::GetLastError());
        return false;
    }

    queueMessage(wxString::FromUTF8("coletando dados da máquina..."));
    extraction.data = {};
    if (!sysinfo::GetMachine(&extraction.data)) {
        sysinfo::Cleanup();
        queueMessage(sysinfo::GetLastError());
        return false;
    }

    queueMessage(wxString::FromUTF8("dados coletados com êxito"));
    extraction.datetime = wxDateTime::Now();
    sysinfo::Cleanup();
    return true;
}