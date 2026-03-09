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
    
    view->Bind(EVT_SYNCFORM, [this](wxCommandEvent&) { model->SendExtractionToServer(); }, ID_SYNC_FORM);

    model->Bind(EVT_APPMODEL_MESSAGE, [this](wxCommandEvent& evt) { view->ShowMessage(evt.GetString()); });
    model->Bind(EVT_APPMODEL_INIT, &AppController::On_AppModel_Init, this);
    model->Bind(EVT_APPMODEL_LOGIN, &AppController::On_AppModel_Login, this);
    model->Bind(EVT_APPMODEL_QUERY_OWNER, &AppController::On_AppModel_QueryOwner, this);
    model->Bind(EVT_APPMODEL_CREATE_USER, &AppController::On_AppModel_CreateUser, this);
    model->Bind(EVT_APPMODEL_SERVER, &AppController::On_AppModel_Server, this);
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
	model->CreateUser(data->cpf, data->name, data->password);
}

void inventory::AppController::On_AuthForm_Machine(wxCommandEvent& event)
{
    view->SetUI(ID_MACHINE_FORM);
}

void inventory::AppController::On_AppModel_Init(wxCommandEvent& event)
{
    if (event.GetInt()) {
        if (model->GetServer()->NoConnectionInLastRequest()) {
            view->SetUI(ID_MACHINE_FORM);
            return;
        }

        view->SetUI(model->GetLoggedUser() ? ID_HOME_FORM : ID_AUTH_FORM);
    }
}

void inventory::AppController::On_AppModel_Login(wxCommandEvent& event)
{
    if (event.GetInt())
        view->SetUI(ID_HOME_FORM);
    else
        view->GetCurrentForm()->BlockChanges(false);
}

void inventory::AppController::On_AppModel_QueryOwner(wxCommandEvent& event)
{
    SyncForm *form = view->GetSyncForm();
    if (!form) // caso a requisição tenha sido solicitada em outra interface
		return;

	form->BlockChanges(false);
    if (event.GetInt())
		form->SetState(SyncForm::READY);
}

void inventory::AppController::On_AppModel_CreateUser(wxCommandEvent& event)
{
	AuthForm* form = view->GetAuthForm();
    if (!form) // caso a requisição tenha sido solicitada em outra interface
        return;

	form->BlockChanges(false);
    if (event.GetInt())
	    form->SetState(AuthForm::UI_AUTH);
}

void inventory::AppController::On_AppModel_Server(wxCommandEvent& event)
{
    SyncForm *form = view->GetSyncForm();
    if (!form) // caso a requisição tenha sido solicitada em outra interface
        return;

    form->BlockChanges(false);
}
