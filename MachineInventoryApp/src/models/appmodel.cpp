#include "models/appmodel.h"

#include <thread>

wxDEFINE_EVENT(EVT_APPMODEL_MESSAGE, wxCommandEvent);
wxDEFINE_EVENT(EVT_APPMODEL_INIT, wxCommandEvent);
wxDEFINE_EVENT(EVT_APPMODEL_LOGIN, wxCommandEvent);

inventory::AppModel::AppModel()
    : wxEvtHandler()
    , isLogged(false)
{}

inventory::AppModel& inventory::AppModel::GetInstance()
{
    static AppModel instance;
    return instance;
}

void inventory::AppModel::Initialize()
{
    std::thread worker([this] {
        queueMessage("inicialiando componentes do sistema...");
        if (!sysinfo::Init()) {
            queueMessage(sysinfo::GetLastError());
            queueInit(0);
            return;
        }
        
        queueMessage(L"inicialiando conexão com servidor...");
        if (!server.Initialize()) {
            queueMessage(wxString::FromUTF8(server.GetLastError()));
            queueInit(0);
            sysinfo::Cleanup();
            return;
        }

        queueMessage(L"coletando dados do usuário...");
        isLogged = false;
        if (server.HasToken()) {
            isLogged = server.GetUser(&loggedUser);
        }

        queueMessage(L"coletando dados da máquina...");
        bool success = true;
        if (!sysinfo::GetMachine(&machine)) {
            queueMessage(sysinfo::GetLastError());
            success = false;
        }

        sysinfo::Cleanup();
        queueInit(success);
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