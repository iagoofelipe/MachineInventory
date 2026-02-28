#include "appcontroller.h"
#include "id.h"

inventory::AppController& inventory::AppController::GetInstance()
{
    static AppController instance;
    return instance;
}

inventory::AppController::AppController()
    : wxEvtHandler()
    , model(AppModel::GetInstance())
    , view(new AppView)
{
    view->Bind(EVT_APPVIEW_LOGOUT, &AppController::On_AppView_Logout, this);
    view->Bind(EVT_AUTHFORM_AUTH, &AppController::On_AuthForm_Auth, this, ID_AUTH_FORM);
    view->Bind(EVT_AUTHFORM_CREATEACCOUNT, &AppController::On_AuthForm_CreateAccount, this, ID_AUTH_FORM);
    view->Bind(EVT_AUTHFORM_MACHINE, &AppController::On_AuthForm_Machine, this, ID_AUTH_FORM);

    model->Bind(EVT_APPMODEL_MESSAGE, &AppController::On_AppModel_Message, this);
    model->Bind(EVT_APPMODEL_INIT, &AppController::On_AppModel_Init, this);
    model->Bind(EVT_APPMODEL_LOGIN, &AppController::On_AppModel_Login, this);
}

void inventory::AppController::Initialize()
{
    view->Initialize();
    model->Initialize();
}

void inventory::AppController::On_AppView_Logout(wxCommandEvent&)
{
    model->Logout();
    view->SetUI(ID_AUTH_FORM);
}

void inventory::AppController::On_AuthForm_Auth(wxCommandEvent& event)
{
    auth* data = (auth*) event.GetClientData();
    model->Auth(data->cpf, data->password);
}

void inventory::AppController::On_AuthForm_CreateAccount(wxCommandEvent& event)
{
    newUser* data = (newUser*) event.GetClientData();
    wxMessageBox("CPF: " + data->cpf + " Name: " + data->name + " Password: " + data->password, "New User Required");
    view->GetAuthForm()->BlockChanges(false);
}

void inventory::AppController::On_AuthForm_Machine(wxCommandEvent& event)
{
    view->SetUI(ID_MACHINE_FORM);
}

void inventory::AppController::On_AppModel_Message(wxCommandEvent& event)
{
    view->ShowMessage(event.GetString(), 5000);
}

void inventory::AppController::On_AppModel_Init(wxCommandEvent& event)
{
    if (event.GetInt())
        view->SetUI(model->GetLoggedUser() ? ID_HOME_FORM : ID_AUTH_FORM);
}

void inventory::AppController::On_AppModel_Login(wxCommandEvent& event)
{
    if (event.GetInt())
        view->SetUI(ID_HOME_FORM);
    else
        view->GetCurrentForm()->BlockChanges(false);
}