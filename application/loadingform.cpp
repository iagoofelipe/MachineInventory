#include "loadingform.h"
#include "id.h"

inventory::LoadingForm::LoadingForm(wxWindow* parent, const wxString& msg)
    : wxPanel(parent, ID_LOADING_FORM)
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddStretchSpacer();
    sizer->Add(lbMessage = new wxStaticText(this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxEXPAND);
    sizer->AddStretchSpacer();

    SetSizer(sizer);
}

void inventory::LoadingForm::ShowMessage(const wxString& msg, int timeout)
{
    lbMessage->SetLabel(msg);
    Layout();
}

void inventory::LoadingForm::BlockChanges(bool block)
{}

void inventory::LoadingForm::Clear()
{}
