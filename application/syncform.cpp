#include "syncform.h"

inventory::SyncForm::SyncForm(wxWindow* parent, wxWindowID id, int border)
    : wxPanel(parent, id)
{
    setupUI(border);
}

void inventory::SyncForm::ShowMessage(const wxString& msg)
{
    lbMessage->SetLabel(msg);
    Layout();
}

void inventory::SyncForm::BlockChanges(bool block)
{}

void inventory::SyncForm::Clear()
{}

void inventory::SyncForm::setupUI(int border)
{
    wxCheckBox *checkbox;
    wxButton *btnBack, *btnContinue;

    wxBoxSizer
        *sizer = new wxBoxSizer(wxVERTICAL),
        *vFormSizer = new wxBoxSizer(wxVERTICAL),
        *hFormSizer = new wxBoxSizer(wxHORIZONTAL),
        *btnsSizer = new wxBoxSizer(wxHORIZONTAL);

    const wxSizerFlags& flags = wxSizerFlags().Expand().Border(wxTOP, 6);
    sizer->Add(lbTitle = new wxStaticText(this, wxID_ANY, "-TITLE-"), 0, wxRIGHT | wxLEFT | wxTOP, border);
    sizer->AddStretchSpacer();
    sizer->Add(hFormSizer, flags);
    sizer->Add(lbMessage = new wxStaticText(this, wxID_ANY, "-MESSAGE-", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), flags);
    sizer->AddStretchSpacer();
    sizer->Add(btnsSizer, 0, wxEXPAND | wxRIGHT | wxLEFT |  wxBOTTOM, border);

    // Form
    hFormSizer->AddStretchSpacer();
    hFormSizer->Add(vFormSizer, 2);
    hFormSizer->AddStretchSpacer();

    vFormSizer->Add(checkbox = new wxCheckBox(this, wxID_ANY, wxString::FromUTF8("Cadastrar máquina de outro usuário")));
    vFormSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("CPF Proprietário")), flags);
    vFormSizer->Add(txtCpf = new wxTextCtrl(this, wxID_ANY), flags);
    vFormSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Nome Proprietário")), flags);
    vFormSizer->Add(txtName = new wxTextCtrl(this, wxID_ANY), flags);
    vFormSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Título da Máquina")), flags);
    vFormSizer->Add(txtMachineTitle = new wxTextCtrl(this, wxID_ANY), flags);

    // Footer Buttons
    btnsSizer->AddStretchSpacer();
    btnsSizer->Add(btnBack = new wxButton(this, wxID_ANY, "voltar"));
    btnsSizer->Add(btnContinue = new wxButton(this, wxID_ANY, "continuar"), 0, wxLEFT, 6);

    // Styles
    wxFont font = lbTitle->GetFont();
    font.SetPointSize(15);
    lbTitle->SetFont(font);

    lbMessage->SetForegroundColour(*wxRED);

    SetSizer(sizer);
}

void inventory::SyncForm::OnBtnBack(wxCommandEvent& event)
{}

void inventory::SyncForm::OnBtnContinue(wxCommandEvent& event)
{}

void inventory::SyncForm::OnCheckbox(wxCommandEvent& event)
{}
