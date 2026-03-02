#include "homeform.h"
#include "id.h"

inventory::HomeForm::HomeForm(wxWindow* parent)
    : wxPanel(parent, ID_HOME_FORM)
{
    setupUI();
}

void inventory::HomeForm::ShowMessage(const wxString& msg, int timeout)
{
}

void inventory::HomeForm::BlockChanges(bool block)
{}

void inventory::HomeForm::Clear()
{}

void inventory::HomeForm::setupUI()
{
    notebook = new wxNotebook(this, wxID_ANY);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    notebook->AddPage(machineForm = new MachineForm(notebook, 10), wxString::FromUTF8("Máquina"));
    notebook->AddPage(syncForm = new SyncForm(notebook, 10), wxString::FromUTF8("Sincronização"));

    sizer->Add(notebook, 1, wxEXPAND);

    SetSizer(sizer);
}