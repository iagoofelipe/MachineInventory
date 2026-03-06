#include "syncform.h"
#include "id.h"

wxDEFINE_EVENT(EVT_SYNCFORM, wxCommandEvent);       // Sincronização requisitada

inventory::SyncForm::SyncForm(wxWindow* parent, int border)
    : wxPanel(parent, ID_SYNC_FORM)
	, m_model(AppModel::GetInstance())
    , m_isBlocked(false)
{
	STATE state = m_model->HasUserRule(sysinfo::ADD_MACHINE) ? GET_OWNER : READY;

    setupUI(border);
    SetState(state, true);

	Bind(wxEVT_CHECKBOX, &SyncForm::OnCheckbox, this, ID_SYNC_FORM_CHECKBOX);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { SetState(m_prevState); }, ID_SYNC_FORM_BTN_BACK);
    Bind(wxEVT_BUTTON, &SyncForm::OnBtnContinue, this, ID_SYNC_FORM_BTN_CONTINUE);
}

void inventory::SyncForm::ShowMessage(const wxString& msg, int timeout)
{
    m_lbMessage->SetLabel(msg);
    Layout();
}

void inventory::SyncForm::BlockChanges(bool block)
{
    if (m_isBlocked == block)
        return;

	// armazenando estado anterior dos campos para restaurar depois
    if (block) {
        m_changesBlocked[m_txtCpf->GetId()] = m_txtCpf->IsEnabled();
        m_changesBlocked[m_txtName->GetId()] = m_txtName->IsEnabled();
        m_changesBlocked[m_txtMachineTitle->GetId()] = m_txtMachineTitle->IsEnabled();
        m_changesBlocked[m_checkbox->GetId()] = m_checkbox->IsEnabled();

        m_txtCpf->Enable(false);
        m_txtName->Enable(false);
        m_txtMachineTitle->Enable(false);
        m_checkbox->Enable(false);
    }
    else {
        m_txtCpf->Enable(m_changesBlocked[m_txtCpf->GetId()]);
        m_txtName->Enable(m_changesBlocked[m_txtName->GetId()]);
        m_txtMachineTitle->Enable(m_changesBlocked[m_txtMachineTitle->GetId()]);
        m_checkbox->Enable(m_changesBlocked[m_checkbox->GetId()]);
    }

    m_isBlocked = block;
    m_btnBack->Enable(!block);
    m_btnContinue->Enable(!block);
}

void inventory::SyncForm::Clear()
{}

void inventory::SyncForm::SetState(STATE state, bool force)
{
	if (m_state == state && !force)
        return;

    bool add_machine_rule = m_model->HasUserRule(sysinfo::ADD_MACHINE);

    if (state == GET_OWNER && !add_machine_rule) {
		ShowMessage(wxString::FromUTF8("Você não tem permissão para cadastrar máquinas de outros usuários"));
        return;
    }

    m_txtCpf->Enable(false);
    m_txtName->Enable(false);
    m_state = state;

    switch (state)
    {
    case GET_OWNER:
    {
		m_lbTitle->SetLabel(wxString::FromUTF8("Dados do Proprietário"));
        SetUser(m_model->GetLoggedUser());

        m_checkbox->Show();
        m_checkbox->SetValue(false);
        m_btnBack->Hide();
        m_lbMachineTitle->Hide();
        m_txtMachineTitle->Hide();
        break;
    }

    case READY:
        m_prevState = GET_OWNER;
        m_lbTitle->SetLabel(wxString::FromUTF8("Dados para Sincronização"));
        SetUser(m_model->GetOwner());

        m_checkbox->Hide();
        m_btnBack->Show(add_machine_rule);
        m_lbName->Show();
        m_txtName->Show();
        m_lbMachineTitle->Show();
		m_txtMachineTitle->Show();
		break;
    }

    Layout();
}

void inventory::SyncForm::SetUser(const user* u)
{
    m_txtCpf->SetValue(u ? u->cpf : (wxString)"");
    m_txtName->SetValue(u ? u->name : (wxString)"");
}

void inventory::SyncForm::setupUI(int border)
{
    wxBoxSizer
        *sizer = new wxBoxSizer(wxVERTICAL),
        *vFormSizer = new wxBoxSizer(wxVERTICAL),
        *hFormSizer = new wxBoxSizer(wxHORIZONTAL),
        *btnsSizer = new wxBoxSizer(wxHORIZONTAL);

    const wxSizerFlags& flags = wxSizerFlags().Expand().Border(wxTOP, 6);
    sizer->Add(m_lbTitle = new wxStaticText(this, wxID_ANY, "-TITLE-"), 0, wxRIGHT | wxLEFT | wxTOP, border);
    sizer->AddStretchSpacer();
    sizer->Add(hFormSizer, flags);
    sizer->Add(m_lbMessage = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), flags);
    sizer->AddStretchSpacer();
    sizer->Add(btnsSizer, 0, wxEXPAND | wxRIGHT | wxLEFT |  wxBOTTOM, border);

    // Form
    hFormSizer->AddStretchSpacer();
    hFormSizer->Add(vFormSizer, 2);
    hFormSizer->AddStretchSpacer();

    vFormSizer->Add(m_checkbox = new wxCheckBox(this, ID_SYNC_FORM_CHECKBOX, wxString::FromUTF8("Cadastrar máquina de outro usuário")));
    vFormSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("CPF Proprietário")), flags);
    vFormSizer->Add(m_txtCpf = new wxTextCtrl(this, wxID_ANY), flags);
    vFormSizer->Add(m_lbName = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Nome Proprietário")), flags);
    vFormSizer->Add(m_txtName = new wxTextCtrl(this, wxID_ANY), flags);
    vFormSizer->Add(m_lbMachineTitle = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Título da Máquina")), flags);
    vFormSizer->Add(m_txtMachineTitle = new wxTextCtrl(this, wxID_ANY), flags);

    // Footer Buttons
    btnsSizer->AddStretchSpacer();
    btnsSizer->Add(m_btnBack = new wxButton(this, ID_SYNC_FORM_BTN_BACK, "voltar"));
    btnsSizer->Add(m_btnContinue = new wxButton(this, ID_SYNC_FORM_BTN_CONTINUE, "continuar"), 0, wxLEFT, 6);

    // Styles
    wxFont font = m_lbTitle->GetFont();
    font.SetPointSize(15);
    m_lbTitle->SetFont(font);

    m_lbMessage->SetForegroundColour(*wxRED);

    SetSizer(sizer);
}

void inventory::SyncForm::OnBtnContinue(wxCommandEvent& event)
{
	m_lbMessage->SetLabel("");

    switch (m_state)
    {

    // verificar se é para outro usuário e requisitar os dados do usuário desejado
    case GET_OWNER:
    {
        if (!m_checkbox->GetValue()) {
            m_model->SetLoggedUserAsOwner();
			SetState(READY);
            return;
        }

        const wxString& cpf = m_txtCpf->GetValue();

        if (cpf.IsEmpty()) {
            ShowMessage("Preencha o CPF para prosseguir!");
            return;
        }

		BlockChanges();
        m_model->QueryOwner(cpf);
        break;
    }

    // enviar dados da máquina
    case READY:
		const wxString& machine_title = m_txtMachineTitle->GetValue();

        if (machine_title.empty()) {
            ShowMessage(wxString::FromUTF8("O título da máquina é obrigatório!"));
			return;
        }

        BlockChanges();
		m_model->SetExtractionTitle(machine_title);
		wxCommandEvent sync_event(EVT_SYNCFORM, GetId());
		ProcessEvent(sync_event);
        break;
    }
}

void inventory::SyncForm::OnCheckbox(wxCommandEvent& event)
{
    bool checked = event.GetInt();
	const user* u = checked? nullptr : m_model->GetLoggedUser();

    m_txtCpf->Enable(checked);
    m_lbName->Show(!checked);
    m_txtName->Show(!checked);

    m_lbMessage->SetLabel("");
    m_txtCpf->SetValue(u ? u->cpf : (wxString)"");
    m_txtName->SetValue(u ? u->name : (wxString)"");
    Layout();
}
