#include "homeform.h"
#include "id.h"

inventory::HomeForm::HomeForm(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
    , syncForm(syncForm)
    , machineForm(machineForm)
{
    setupUI();
}

void inventory::HomeForm::ShowMessage(const wxString& msg)
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

    notebook->AddPage(machineForm = new MachineForm(notebook, ID_MACHINE_FORM, 10), wxString::FromUTF8("Máquina"));
    notebook->AddPage(syncForm = new SyncForm(notebook, ID_SYNC_FORM, 10), wxString::FromUTF8("Sincronização"));

    sizer->Add(notebook, 1, wxEXPAND);

    SetSizer(sizer);
}

// wxPanel* inventory::HomeForm::addPage(wxWindow* page, const wxString& title)
// {
//     wxPanel *panel = new wxPanel(notebook);
//     wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

//     page->Reparent(panel);
//     sizer->Add(page, 1, wxEXPAND | wxALL, 6);
//     panel->SetSizer(sizer);
    
//     notebook->AddPage(panel, title);

//     return panel;
// }