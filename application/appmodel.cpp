#include "appmodel.h"

#include <thread>

wxDEFINE_EVENT(EVT_APPMODEL_MESSAGE, wxCommandEvent);
wxDEFINE_EVENT(EVT_APPMODEL_INIT, wxCommandEvent);
wxDEFINE_EVENT(EVT_APPMODEL_LOGIN, wxCommandEvent);
wxDEFINE_EVENT(EVT_APPMODEL_MACHINE, wxCommandEvent);

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
        } else
            isLogged = true;

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
        int success = extractMachine();
        
        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_MACHINE);
        evt->SetInt(success);
        evt->SetClientData(success? (void*)GetExtraction() : nullptr);
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