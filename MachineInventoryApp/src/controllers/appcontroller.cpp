#include "controllers/appcontroller.h"

inventory::AppController& inventory::AppController::GetInstance()
{
    static AppController instance;
    return instance;
}

inventory::AppController::AppController()
    : wxEvtHandler()
    , model(AppModel::GetInstance())
    , view(AppView::GetInstance())
{
    view.Bind(EVT_APPVIEW_UI, &AppController::On_AppView_UI, this);
    view.Bind(EVT_APPVIEW_LOGOUT, &AppController::On_AppView_Logout, this);

    model.Bind(EVT_APPMODEL_MESSAGE, &AppController::On_AppModel_Message, this);
    model.Bind(EVT_APPMODEL_INIT, &AppController::On_AppModel_Init, this);
    model.Bind(EVT_APPMODEL_LOGIN, &AppController::On_AppModel_Login, this);
}

void inventory::AppController::Initialize()
{
    view.Initialize();
    model.Initialize();
}

void inventory::AppController::On_AppView_UI(wxCommandEvent& event)
{
    AppView::UI ui = (AppView::UI)event.GetInt();

    if (ui & AppView::AUTH) {
        AuthForm* form = view.GetAuthForm();
        form->Bind(EVT_AUTHFORM_AUTH, &AppController::On_AuthForm_Auth, this);
        form->Bind(EVT_AUTHFORM_CREATEACCOUNT, &AppController::On_AuthForm_CreateAccount, this);
        form->Bind(EVT_AUTHFORM_MACHINE, &AppController::On_AuthForm_Machine, this);
    }

    if (ui & AppView::HOME) {
        HomeForm* form = view.GetHomeForm();
        // form->Bind(EVT_HOMEFORM_LOGOUT, &AppController::On_HomeForm_Logout, this);
    }
}

void inventory::AppController::On_AppView_Logout(wxCommandEvent&)
{
    model.Logout();
    view.SetUI(AppView::AUTH);
}

void inventory::AppController::On_AuthForm_Auth(wxCommandEvent& event)
{
    auth* data = (auth*) event.GetClientData();
    model.Auth(data->cpf, data->password);
}

void inventory::AppController::On_AuthForm_CreateAccount(wxCommandEvent& event)
{
    newUser* data = (newUser*) event.GetClientData();
    wxMessageBox("CPF: " + data->cpf + " Name: " + data->name + " Password: " + data->password, "New User Required");
    view.GetAuthForm()->BlockChanges(false);
}

void inventory::AppController::On_AuthForm_Machine(wxCommandEvent& event)
{
    view.SetUI(AppView::MACHINE);
}

void inventory::AppController::On_AppModel_Message(wxCommandEvent& event)
{
    IForm* form;
    if (form = view.GetCurrentForm())
        form->ShowMessage(event.GetString());
}

void inventory::AppController::On_AppModel_Init(wxCommandEvent& event)
{
    if (event.GetInt())
        view.SetUI(model.GetLoggedUser() ? AppView::HOME : AppView::AUTH);
}

void inventory::AppController::On_AppModel_Login(wxCommandEvent& event)
{
    if (event.GetInt())
        view.SetUI(AppView::HOME);
    else
        view.GetCurrentForm()->BlockChanges(false);
}