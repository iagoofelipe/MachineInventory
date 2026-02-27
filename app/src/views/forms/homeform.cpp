#include "views/forms/homeform.h"

inventory::HomeForm::HomeForm(wxWindow* parent, SyncForm* syncForm, MachineForm* machineForm)
    : wxPanel(parent)
    , syncForm(syncForm)
    , machineForm(machineForm)
    , syncFormGenerated(!syncForm)
    , machineFormGenerated(!machineForm)
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

    if (!syncForm)
        syncForm = new SyncForm(NULL);
    
    if (!machineForm)
        machineForm = new MachineForm(NULL);

    addPage(machineForm, L"Máquina");
    addPage(syncForm, L"Sincronização");

    sizer->Add(notebook, 1, wxEXPAND);

    SetSizer(sizer);
}

wxPanel* inventory::HomeForm::addPage(wxWindow* page, const wxString& title)
{
    wxPanel *panel = new wxPanel(notebook);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    page->Reparent(panel);
    sizer->Add(page, 1, wxEXPAND | wxALL, 6);
    panel->SetSizer(sizer);
    
    notebook->AddPage(panel, title);

    return panel;
}