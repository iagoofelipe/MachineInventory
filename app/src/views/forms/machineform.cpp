#include "views/forms/machineform.h"

#include <wx/splitter.h>

inventory::MachineForm::MachineForm(wxWindow* parent)
    : wxPanel(parent)
    , model(AppModel::GetInstance())
{
    setupUI();
}

void inventory::MachineForm::ShowMessage(const wxString& msg)
{}

void inventory::MachineForm::BlockChanges(bool block)
{}

void inventory::MachineForm::Clear()
{}

void inventory::MachineForm::setupUI()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);
    
    tree = new wxTreeCtrl(splitter, wxID_ANY);
    tree->Bind(wxEVT_TREE_SEL_CHANGED, &MachineForm::OnTreeSelChange, this);

    wxTreeItemId root = tree->AddRoot("Resumo do Sistema");
    wxTreeItemId comp = tree->AppendItem(root, "Componentes");
    tree->AppendItem(comp, L"Multimídia");
    tree->AppendItem(comp, "Rede");
    
    wxTreeItemId env = tree->AppendItem(root, "Ambiente de Software");
    tree->AppendItem(env, L"Variáveis de Ambiente");

    list = new wxListCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    list->InsertColumn(0, "Item", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, "Valor", wxLIST_FORMAT_LEFT, 400);

    sizer->Add(splitter, 1, wxEXPAND | wxALL, 6);
    splitter->SplitVertically(tree, list, 250);
    tree->Expand(root);

    if (!tree->IsSelected(root))
        tree->SelectItem(root);

    SetSizer(sizer);
}

void inventory::MachineForm::setupSummary()
{
    long i;
    sysinfo::machine *machine = model.GetMachine();

    // Rows
    i = list->InsertItem(0, L"Processador");
    list->SetItem(i, 1, machine->processorName);

    i = list->InsertItem(0, L"Clock do Processador");
    list->SetItem(i, 1, std::to_string(machine->processorClockSpeed));

    i = list->InsertItem(0, L"Organização");
    list->SetItem(i, 1, machine->osOrganization);

    i = list->InsertItem(0, L"Fabricante da Placa Mãe");
    list->SetItem(i, 1, machine->motherboardManufacturer);

    i = list->InsertItem(0, L"Placa Mãe");
    list->SetItem(i, 1, machine->motherboardName);

    i = list->InsertItem(0, L"Número de Série");
    list->SetItem(i, 1, machine->osSerialNumber);

    i = list->InsertItem(0, L"Data de Instalação");
    list->SetItem(i, 1, machine->osInstallDate);

    i = list->InsertItem(0, L"Arquitetura");
    list->SetItem(i, 1, machine->osArchitecture);

    i = list->InsertItem(0, L"Versão do Sistema");
    list->SetItem(i, 1, machine->osVersion);

    i = list->InsertItem(0, L"Sistema Operacional");
    list->SetItem(i, 1, machine->osName);

}

void inventory::MachineForm::OnTreeSelChange(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    if (!id.IsOk()) {
        return;
    }

    list->Freeze();
    list->DeleteAllItems();

    const wxString& label = tree->GetItemText(id);
    if (label == "Resumo do Sistema") {
        setupSummary();
    }

    list->Thaw();
}

