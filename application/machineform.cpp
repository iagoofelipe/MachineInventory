#include "machineform.h"
#include "appmodel.h"

#include <sysinfo/sysinfo.h>
#include <wx/splitter.h>

inventory::MachineForm::MachineForm(wxWindow* parent, wxWindowID id, int border)
    : wxPanel(parent, id)
    , model(AppModel::GetInstance())
{
    setupUI(border);
    
    Bind(wxEVT_BUTTON, &MachineForm::OnBtnSync, this, ID_BTN_SYNC);
    model->Bind(EVT_APPMODEL_MACHINE, &MachineForm::OnAppModelMachine, this);
}

void inventory::MachineForm::ShowMessage(const wxString& msg)
{}

void inventory::MachineForm::BlockChanges(bool block)
{
    btnSync->Enable(!block);
}

void inventory::MachineForm::Clear()
{}

void inventory::MachineForm::UpdateTreeSelection()
{
    UpdateTreeSelection(tree->GetSelection());
}

void inventory::MachineForm::UpdateTreeSelection(wxTreeItemId id)
{
    if (!id.IsOk()) {
        return;
    }

    list->Freeze();
    list->DeleteAllItems();

    const wxString& label = tree->GetItemText(id);
    if (label == "Resumo do Sistema")
        setupSummary();
    else if (label == "Rede")
        setupNewtwork();
    else if (label == "Armazenamento")
        setupDisks();
    else if (label == wxString::FromUTF8("Memória RAM"))
        setupMemory();
    else if (label == "Programas")
        setupPrograms();

    list->Thaw();
}

void inventory::MachineForm::setupUI(int border)
{
    const machine_extraction* extraction = model->GetExtraction();

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* footerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);
    
    tree = new wxTreeCtrl(splitter, wxID_ANY);
    tree->Bind(wxEVT_TREE_SEL_CHANGED, [this](wxTreeEvent& evt) { UpdateTreeSelection(evt.GetItem()); });

    wxTreeItemId root = tree->AddRoot(L"Resumo do Sistema");
    wxTreeItemId hardware = tree->AppendItem(root, "Componentes de Hardware");
    tree->AppendItem(hardware, "Rede");
    tree->AppendItem(hardware, "Armazenamento");
    tree->AppendItem(hardware, wxString::FromUTF8("Memória RAM"));
    
    wxTreeItemId software = tree->AppendItem(root, "Componentes de Software");
    tree->AppendItem(software, "Programas");

    list = new wxListCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    list->InsertColumn(0, "Item", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, "Valor", wxLIST_FORMAT_LEFT, 400);

    sizer->Add(splitter, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, border);
    sizer->AddSpacer(6);
    sizer->Add(footerSizer, 0, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, border);

    const wxString& lastSyncText = extraction->datetime.Format("Sincronizado em %H:%M %d/%m");
    footerSizer->Add(lastSync = new wxStaticText(this, wxID_ANY, lastSyncText), 0, wxALIGN_CENTER_VERTICAL);
    footerSizer->AddStretchSpacer();
    footerSizer->Add(btnSync = new wxButton(this, ID_BTN_SYNC, "sincronizar"));

    splitter->SplitVertically(tree, list, 250);

    tree->Expand(root);
    tree->Expand(software);
    tree->Expand(hardware);

    if (!tree->IsSelected(root))
        tree->SelectItem(root);

    SetSizer(sizer);
}

void inventory::MachineForm::setupSummary()
{
    long i;
    const sysinfo::machine *machine = model->GetMachine();

    i = list->InsertItem(0, "Processador");
    list->SetItem(i, 1, machine->processorName);

    i = list->InsertItem(0, "Clock do Processador");
    list->SetItem(i, 1, machine->processorClockSpeed);

    i = list->InsertItem(0, wxString::FromUTF8("Organização"));
    list->SetItem(i, 1, machine->osOrganization);

    i = list->InsertItem(0, wxString::FromUTF8("Fabricante da Placa Mãe"));
    list->SetItem(i, 1, machine->motherboardManufacturer);

    i = list->InsertItem(0, wxString::FromUTF8("Placa Mãe"));
    list->SetItem(i, 1, machine->motherboardName);

    i = list->InsertItem(0, wxString::FromUTF8("Número de Série"));
    list->SetItem(i, 1, machine->osSerialNumber);

    i = list->InsertItem(0, wxString::FromUTF8("Data de Instalação"));
    list->SetItem(i, 1, machine->osInstallDate);

    i = list->InsertItem(0, "Arquitetura");
    list->SetItem(i, 1, machine->osArchitecture);

    i = list->InsertItem(0, wxString::FromUTF8("Versão do Sistema"));
    list->SetItem(i, 1, machine->osVersion);

    i = list->InsertItem(0, "Sistema Operacional");
    list->SetItem(i, 1, machine->osName);

}

void inventory::MachineForm::setupNewtwork()
{
    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->network_adapters.size(); j > 0; --j) {
        const sysinfo::network_adapter& adapter = machine->network_adapters[j - 1];

        i = list->InsertItem(0, "Mac");
        list->SetItem(i, 1, adapter.mac);

        i = list->InsertItem(0, "Nome");
        list->SetItem(i, 1, adapter.name);

        if (j != 1)
            i = list->InsertItem(0, "");
    }
}

void inventory::MachineForm::setupDisks()
{
    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->disks.size(); j > 0; --j) {
        const sysinfo::disk& disk = machine->disks[j - 1];

        i = list->InsertItem(0, "Model");
        list->SetItem(i, 1, disk.model);

        i = list->InsertItem(0, wxString::FromUTF8("Espaço"));
        list->SetItem(i, 1, disk.size);

        i = list->InsertItem(0, wxString::FromUTF8("Número de Série"));
        list->SetItem(i, 1, disk.seriaNumber);

        i = list->InsertItem(0, "Nome");
        list->SetItem(i, 1, disk.name);

        if (j != 1)
            i = list->InsertItem(0, "");
    }
}

void inventory::MachineForm::setupMemory()
{
    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->physical_memories.size(); j > 0; --j) {
        const sysinfo::physical_memory& memory = machine->physical_memories[j - 1];

        i = list->InsertItem(0, "Velocidade");
        list->SetItem(i, 1, memory.speed);

        i = list->InsertItem(0, "Capacidade");
        list->SetItem(i, 1, memory.capacity);

        i = list->InsertItem(0, "Nome");
        list->SetItem(i, 1, memory.name);

        if (j != 1)
            i = list->InsertItem(0, "");
    }
}

void inventory::MachineForm::setupPrograms()
{
    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->programs.size(); j > 0; --j) {
        const sysinfo::program& program = machine->programs[j - 1];

        i = list->InsertItem(0, wxString::FromUTF8("Apenas no Usuário Atual"));
        list->SetItem(i, 1, program.CurrentUserOnly ? wxString::FromUTF8("Sim") : wxString::FromUTF8("Não"));
        
        i = list->InsertItem(0, wxString::FromUTF8("Espaço"));
        list->SetItem(i, 1, program.EstimatedSize);
        
        i = list->InsertItem(0, "Provedor");
        list->SetItem(i, 1, program.Publisher);

        i = list->InsertItem(0, wxString::FromUTF8("Versão"));
        list->SetItem(i, 1, program.DisplayVersion);

        i = list->InsertItem(0, "Nome");
        list->SetItem(i, 1, program.DisplayName);

        if (j != 1)
            i = list->InsertItem(0, "");
    }
}

void inventory::MachineForm::OnBtnSync(wxCommandEvent& event)
{
    BlockChanges();
    model->UpdateExtraction();

    // wxCommandEvent evt(EVT_MACHINEFORM_SYNC, GetId());
    // ProcessEvent(evt);
}

void inventory::MachineForm::OnAppModelMachine(wxCommandEvent& event)
{
    BlockChanges(false);

    if (!event.GetInt())
        return;
        
    const machine_extraction* extraction = (const machine_extraction*)event.GetClientData();
    lastSync->SetLabel(extraction->datetime.Format("Sincronizado em %H:%M %d/%m"));

    UpdateTreeSelection();
}