#include "views/forms/authform.h"

wxDEFINE_EVENT(EVT_AUTHFORM_AUTH, wxCommandEvent);
wxDEFINE_EVENT(EVT_AUTHFORM_CREATEACCOUNT, wxCommandEvent);
wxDEFINE_EVENT(EVT_AUTHFORM_MACHINE, wxCommandEvent);

inventory::AuthForm::AuthForm(wxWindow* parent, State state)
    : wxPanel(parent, wxID_ANY)
    , model(AppModel::GetInstance())
    , currentState(NONE)
{
    setupUI();
    SetState(state);
}

void inventory::AuthForm::SetState(State state)
{
    if (currentState == state)
    return;
    
    currentState = state;
    Clear();

    switch (state)
    {
    case AUTH:
        txtName->Hide();
        lbName->Hide();
        txtPassConfirm->Hide();
        lbPassConfirm->Hide();
        btnAccess->Show();
        btnMachine->Show();
        btnBack->Hide();
        break;

    case CREATE_ACCOUNT:
        txtName->Show();
        lbName->Show();
        txtPassConfirm->Show();
        lbPassConfirm->Show();
        btnAccess->Hide();
        btnMachine->Hide();
        btnBack->Show();
        break;
    }

    Layout();
}

void inventory::AuthForm::ShowMessage(const wxString& msg)
{
    lbMessage->SetLabel(msg);
    Layout();
}

void inventory::AuthForm::BlockChanges(bool block)
{
    txtCpf->Enable(!block);
    txtName->Enable(!block);
    txtPass->Enable(!block);
    txtPassConfirm->Enable(!block);
    btnAccess->Enable(!block);
    btnCreateAccount->Enable(!block);
    btnBack->Enable(!block);
    btnMachine->Enable(!block);
}

void inventory::AuthForm::Clear()
{
    txtCpf->Clear();
    txtName->Clear();
    txtPass->Clear();
    txtPassConfirm->Clear();
    lbMessage->SetLabel("");
}

void inventory::AuthForm::setupUI()
{
    wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSizerFlags& flags = wxSizerFlags().Expand().Border(wxTOP, 6);

    vSizer->AddStretchSpacer();
    vSizer->Add(lbTitle = new wxStaticText(this, wxID_ANY, _T("Inventário de Máquinas"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxEXPAND);
    vSizer->Add(lbCpf = new wxStaticText(this, wxID_ANY, "CPF"), flags);
    vSizer->Add(txtCpf = new wxTextCtrl(this, wxID_ANY), flags);
    vSizer->Add(lbName = new wxStaticText(this, wxID_ANY, "Nome"), flags);
    vSizer->Add(txtName = new wxTextCtrl(this, wxID_ANY), flags);
    vSizer->Add(lbPass = new wxStaticText(this, wxID_ANY, "Senha"), flags);
    vSizer->Add(txtPass = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD), flags);
    vSizer->Add(lbPassConfirm = new wxStaticText(this, wxID_ANY, "Confirmar Senha"), flags);
    vSizer->Add(txtPassConfirm = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD), flags);
    vSizer->Add(lbMessage = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), flags);
    vSizer->Add(btnAccess = new wxButton(this, wxID_ANY, "acessar"), flags);
    vSizer->Add(btnCreateAccount = new wxButton(this, wxID_ANY, "criar conta"), flags);
    vSizer->Add(btnBack = new wxButton(this, wxID_ANY, "voltar"), flags);
    vSizer->Add(btnMachine = new wxButton(this, wxID_ANY, _T("dados da máquina (leitura)")), flags);
    vSizer->AddStretchSpacer();

    hSizer->AddStretchSpacer(1);
    hSizer->Add(vSizer, 2, wxEXPAND);
    hSizer->AddStretchSpacer(1);

    // definindo estilos
    lbMessage->SetForegroundColour(*wxRED);
    
    wxFont font = lbTitle->GetFont();
    font.SetPointSize(15);
    lbTitle->SetFont(font);

    // conectando eventos
    btnAccess->Bind(wxEVT_BUTTON, &AuthForm::OnBtnAccess, this);
    btnCreateAccount->Bind(wxEVT_BUTTON, &AuthForm::OnBtnCreateAccount, this);
    btnBack->Bind(wxEVT_BUTTON, &AuthForm::OnBtnBack, this);
    btnMachine->Bind(wxEVT_BUTTON, &AuthForm::OnBtnMachine, this);

    SetSizer(hSizer);
}

void inventory::AuthForm::OnBtnAccess(wxCommandEvent& event)
{
    authData.cpf = txtCpf->GetValue();
    authData.password = txtPass->GetValue();

    if (authData.cpf.IsEmpty() || authData.password.IsEmpty()) {
        ShowMessage("Preencha todos os campos para prosseguir!");
        return;
    }

    BlockChanges();
    
    wxCommandEvent evt(EVT_AUTHFORM_AUTH, GetId());
    evt.SetClientData(&authData);
    ProcessEvent(evt);
}

void inventory::AuthForm::OnBtnCreateAccount(wxCommandEvent& event)
{
    switch (currentState)
    {
    case AUTH:
        SetState(CREATE_ACCOUNT);
        break;

    case CREATE_ACCOUNT:
        newUserData.cpf = txtCpf->GetValue();
        newUserData.name = txtName->GetValue();
        newUserData.password = txtPass->GetValue();

        if (newUserData.cpf.IsEmpty() || newUserData.name.IsEmpty() || newUserData.password.IsEmpty()) {
            ShowMessage("Preencha todos os campos para prosseguir!");
            return;
        }

        if (newUserData.password != txtPassConfirm->GetValue()) {
            ShowMessage(_T("As senhas são diferentes!"));
            return;
        }

        BlockChanges();

        wxCommandEvent evt(EVT_AUTHFORM_CREATEACCOUNT, GetId());
        evt.SetClientData(&newUserData);
        ProcessEvent(evt);
        break;
    }
}

void inventory::AuthForm::OnBtnBack(wxCommandEvent& event)
{
    SetState(AUTH);
}

void inventory::AuthForm::OnBtnMachine(wxCommandEvent& event)
{
    wxCommandEvent evt(EVT_AUTHFORM_MACHINE, GetId());
    ProcessEvent(evt);
}
