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
wxDEFINE_EVENT(EVT_APPMODEL_SERVER_CONN, wxCommandEvent);   // Conexão estabelecida com o servidor
wxDEFINE_EVENT(EVT_APPMODEL_SERVER_LOST, wxCommandEvent);   // Conexão perdida com o servidor

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
    // configurando logs
    char* buffer = std::getenv("TMP");
    std::string filename = (buffer? buffer : ".") + (std::string)"\\MachineInventoryLog.log";
    logFile = fopen(filename.c_str(), "a");

    if (logFile) {
        wxLogStderr* f = new wxLogStderr(logFile);
        wxLog::SetActiveTarget(f);
        wxLog::AddTraceMask("TIMESTAMP");
    } else {
        wxLogError("Não foi possível criar/abrir o arquivo de log!");
    }

    std::thread worker([this] {
        if (!extractMachine()) {
            queueInit(0);
            return;
        }

        queueMessage(wxString::FromUTF8("inicialiando conexão com servidor..."));
        if (!server.Initialize()) {
            queueError(wxString::FromUTF8(server.GetLastError()));
            queueInit(0);
            return;
        }

        queueMessage(wxString::FromUTF8("coletando dados do usuário..."));
        isLogged = false;
        sysinfo::user u;
        if (server.HasToken()) { // garante que pelo menos uma requisição será feita ao servidor
            if (isLogged = server.GetUser(&u)) {
                owner = loggedUser = u;

            }
        }
        else {
            server.TestConnection();
        }

        queueInit(1);
    });

    worker.detach();
}

void inventory::AppModel::Cleanup()
{
    delete wxLog::SetActiveTarget(nullptr);
    if (logFile) {
        fclose(logFile);
        logFile = nullptr;
    }
}

void inventory::AppModel::Auth(const wxString& cpf, const wxString& password)
{
    std::thread worker([this, cpf, password] {
        sysinfo::user u;
        if (
            !server.Auth(cpf.utf8_str().data(), password.utf8_str().data()) ||
            !server.GetUser(&u)
        ) {
            isLogged = false;
            queueError(wxString::FromUTF8(server.GetLastError()));
        }
        else {
            isLogged = true;
            owner = loggedUser = u;
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
        int success = server.UploadMachine(&extraction.data, owner.cpf.utf8_str().data(), extraction.title.utf8_str().data());
        
        if (!success)
            queueError(wxString::Format("erro ao enviar os dados: %s", server.GetLastError()));
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
        sysinfo::user u;
        int success;

        if (loggedUser.cpf == cpf) {
            if (owner.id != loggedUser.id)
                owner = loggedUser;
            
            success = 1;
        }
        else if (server.GetUser(&u, cpf.utf8_str().data())) {
            owner = u;
            success = 1;
        }
        else {
            success = 0;
            queueError(wxString::FromUTF8(server.GetLastError()));
        }

        evt->SetInt(success);
        evt->SetClientData(success? (void*)GetOwner() : nullptr);
        QueueEvent(evt);
    });

	worker.detach();
}

void inventory::AppModel::CreateUser(const wxString& cpf, const wxString& name, const wxString& password)
{
    std::thread worker([this, cpf, name, password] {
        wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_CREATE_USER);
        int success;

        if (!(success = server.CreateNewUser(cpf.utf8_str().data(), name.utf8_str().data(), password.utf8_str().data())))
            queueError(wxString::FromUTF8(server.GetLastError()));

        evt->SetInt(success);
		QueueEvent(evt);
    });

    worker.detach();
}

void inventory::AppModel::SetLoggedUserAsOwner()
{
    owner = loggedUser;
}

void inventory::AppModel::queueMessage(const wxString& msg)
{
    wxCommandEvent* evt = new wxCommandEvent(EVT_APPMODEL_MESSAGE);
    evt->SetString(msg);
    QueueEvent(evt);
}

void inventory::AppModel::queueError(const wxString& msg)
{
    wxLogError(msg);
    queueMessage(msg);
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