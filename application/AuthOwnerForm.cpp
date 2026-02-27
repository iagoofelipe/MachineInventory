#include "AuthOwnerForm.h"

AuthOwnerForm::AuthOwnerForm(wxWindow* parent, sysinfo::ServerAPI* server, AuthOwnerFormState initial_state)
	: wxPanel(parent, wxID_ANY)
	, server(server)
	, currentState(NONE)
	, secondaryState(NONE)
{
	setupUI();
    
    checkbox->Bind(wxEVT_CHECKBOX, &AuthOwnerForm::on_checkbox, this);
    btnSecondary->Bind(wxEVT_BUTTON, &AuthOwnerForm::on_btnSecondary, this);
    btnContinue->Bind(wxEVT_BUTTON, &AuthOwnerForm::on_btnContinue, this);

	if (initial_state != NONE)
        setState(initial_state);
}

void AuthOwnerForm::setState(AuthOwnerFormState state)
{
	currentState = state;

	switch (state)
	{
	case AUTH:
		checkbox->Hide();
		lbCpf->Show();
		txtCpf->Show();
		lbPass->Show();
		txtPass->Show();
		lbPassConfirm->Hide();
		txtPassConfirm->Hide();
		lbName->Hide();
		txtName->Hide();
		//widBtns->Show();
		btnSecondary->Show();
		btnContinue->Show();

		txtCpf->Enable();
		lbMessage->SetLabel("");
		lbTitle->SetLabel(_T("Autenticação do Usuário"));
		btnSecondary->SetLabel(_T("novo usuário"));
		secondaryState = NEW_USER;
		break;

    case NEW_USER:
        checkbox->Hide();
        lbCpf->Show();
        txtCpf->Show();
        lbPass->Show();
        txtPass->Show();
        lbPassConfirm->Show();
        txtPassConfirm->Show();
        lbName->Show();
        txtName->Show();
        //widBtns->Show();
        btnSecondary->Show();
        btnContinue->Show();

        lbMessage->SetLabel("");
        lbTitle->SetLabel(_T("Cadastro de Usuário"));
        btnSecondary->SetLabel("tela de login");
        secondaryState = AUTH;
        break;

    case MACHINE_OWNER:
        checkbox->Show();
        lbCpf->Show();
        txtCpf->Show();
        lbPass->Hide();
        txtPass->Hide();
        lbPassConfirm->Hide();
        txtPassConfirm->Hide();
        lbName->Show();
        txtName->Show();
        //widBtns->Show();
        btnSecondary->Show();
        btnContinue->Show();

        lbMessage->SetLabel("");
        lbTitle->SetLabel(_T("Dados do Proprietário"));
        btnSecondary->SetLabel("tela de login");
        txtCpf->Disable();
        txtName->Disable();
        checkbox->SetValue(false);
        secondaryState = AUTH;
        break;

    case READY_TO_SYNC:
        checkbox->Hide();
        lbCpf->Show();
        txtCpf->Show();
        lbPass->Hide();
        txtPass->Hide();
        lbPassConfirm->Hide();
        txtPassConfirm->Hide();
        lbName->Show();
        txtName->Show();
        //widBtns->Show();
        btnSecondary->Show();
        btnContinue->Show();

        lbMessage->SetLabel("");
        lbTitle->SetLabel(_T("Dados para Sincronização"));
        txtCpf->Disable();
        txtName->Disable();
        btnSecondary->SetLabel("voltar");
        secondaryState = MACHINE_OWNER;
        break;
    }

    Layout();
}

void AuthOwnerForm::setName(const wxString& text)
{
    txtName->SetValue(text);    
}

void AuthOwnerForm::setCpf(const wxString& text)
{
    txtCpf->SetValue(text);    
}

void AuthOwnerForm::setupUI()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	int border = 6;

	sizer->Add(lbTitle = new wxStaticText(this, wxID_ANY, "TITLE"), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(checkbox = new wxCheckBox(this, wxID_ANY, _T("Cadastrar máquina de outro usuário")), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(lbCpf = new wxStaticText(this, wxID_ANY, "CPF"), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(txtCpf = new wxTextCtrl(this, wxID_ANY), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(lbPass = new wxStaticText(this, wxID_ANY, "Senha"), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(txtPass = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(lbPassConfirm = new wxStaticText(this, wxID_ANY, "Confirmar Senha"), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(txtPassConfirm = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(lbName = new wxStaticText(this, wxID_ANY, "Nome"), 0, wxEXPAND | wxBOTTOM, border);
	sizer->Add(txtName = new wxTextCtrl(this, wxID_ANY), 0, wxEXPAND | wxBOTTOM, border);
	sizer->AddStretchSpacer();
	sizer->Add(lbMessage = new wxStaticText(this, wxID_ANY, "MESSAGE"), 0, wxEXPAND | wxBOTTOM, border);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->AddStretchSpacer();
	sizerBtns->Add(btnSecondary = new wxButton(this, wxID_ANY, "secondary"), 0, wxLEFT, border);
	sizerBtns->Add(btnContinue = new wxButton(this, wxID_ANY, "continuar"), 0, wxLEFT, border);

	sizer->Add(sizerBtns, 0, wxEXPAND);

	SetSizer(sizer);
}

void AuthOwnerForm::on_checkbox(wxCommandEvent& event)
{
	bool is_checked = checkbox->GetValue();

    switch (currentState)
	{
    case MACHINE_OWNER:
        txtCpf->Enable(is_checked);
		lbName->Show(!is_checked);
        txtName->Show(!is_checked);
        Layout();
        break;
    }
}

void AuthOwnerForm::on_btnSecondary(wxCommandEvent& event)
{
	if (secondaryState != NONE)
		setState(secondaryState);
}

void AuthOwnerForm::on_btnContinue(wxCommandEvent& event)
{
    sysinfo::user owner;
	lbMessage->SetLabel("");
    // blockButtons(true);

    switch (currentState)
    {
	case AUTH:
    {
	    wxString
            cpf = txtCpf->GetValue(),
            password = txtPass->GetValue();
        

		if (cpf.empty() || password.empty()) {
            lbMessage->SetLabel("CPF e senha são obrigatórios!");
            // blockButtons(false);
            return;
        }

        if (
            !server->Auth(cpf.ToStdString(), password.ToStdString()) ||
            !server->GetUser(&owner)
            ) {
            lbMessage->SetLabel(wxString::FromUTF8(server->GetLastError()));
            // blockButtons(false);
            return;
        }


		txtName->SetValue(wxString::FromUTF8(owner.name));
		setState(MACHINE_OWNER);
        break;
    }


    // verificar se é para outro usuário e requisitar os dados do usuário desejado
    case MACHINE_OWNER:
        if (checkbox->GetValue()) {
            if (!server->GetUser(txtCpf->GetValue().ToStdString(), &owner)) {
                lbMessage->SetLabel(wxString::FromUTF8(server->GetLastError()));
                // blockButtons(false);
                return;
            }

            txtCpf->SetValue(wxString::FromUTF8(owner.cpf));
            txtName->SetValue(wxString::FromUTF8(owner.name));
        }

        setState(READY_TO_SYNC);
        break;

    // enviar dados da máquina
    case READY_TO_SYNC:
		btnContinue->Disable();
		btnSecondary->Disable();
        lbMessage->SetLabel("máquina sincronizada com êxito!");
        break;
    }
    
    // blockButtons(false);
}
