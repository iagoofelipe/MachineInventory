#include "loadingform.h"

inventory::LoadingForm::LoadingForm(wxWindow* parent, wxWindowID id, const wxString& msg)
    : wxPanel(parent, id)
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddStretchSpacer();
    sizer->Add(lbMessage = new wxStaticText(this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxEXPAND);
    sizer->AddStretchSpacer();

    SetSizer(sizer);
}

void inventory::LoadingForm::ShowMessage(const wxString& msg)
{
    lbMessage->SetLabel(msg);
    Layout();
}

void inventory::LoadingForm::BlockChanges(bool block)
{}

void inventory::LoadingForm::Clear()
{}
