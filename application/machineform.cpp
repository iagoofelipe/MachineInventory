#include "machineform.h"
#include "appmodel.h"
#include "id.h"

#include <sysinfo/sysinfo.h>
#include <wx/splitter.h>

inventory::MachineForm::MachineForm(wxWindow* parent, int border)
    : wxPanel(parent, ID_MACHINE_FORM)
    , model(AppModel::GetInstance())
    , syncTextFormat(wxString::FromUTF8("Sincronizado em %d/%m às %H:%M"))
{
    setupUI(border);
    
    Bind(wxEVT_BUTTON, &MachineForm::OnBtnSync, this, ID_MACHINE_FORM_BTN_SYNC);
    model->Bind(EVT_APPMODEL_MACHINE, &MachineForm::OnAppModelMachine, this);
}

void inventory::MachineForm::ShowMessage(const wxString& msg, int timeout)
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
	list->DeleteAllColumns();

    const wxString& label = tree->GetItemText(id);
    const wxString& lbRAM = wxString::FromUTF8("Memória RAM");
    const wxString& lbAccounts = wxString::FromUTF8("Usuários");

    if (label == "Resumo do Sistema")       setupSummary();
    else if (label == "Rede")               setupNewtwork();
    else if (label == "Armazenamento")      setupDisks();
    else if (label == lbRAM)                setupMemory();
    else if (label == "Programas")          setupPrograms();
    else if (label == lbAccounts)           setupAccounts();

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
    tree->AppendItem(software, wxString::FromUTF8("Usuários"));

    list = new wxListCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    

    sizer->Add(splitter, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, border);
    sizer->AddSpacer(6);
    sizer->Add(footerSizer, 0, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, border);

    const wxString& lastSyncText = extraction->datetime.Format(syncTextFormat);
    footerSizer->Add(lastSync = new wxStaticText(this, wxID_ANY, lastSyncText), 0, wxALIGN_CENTER_VERTICAL);
    footerSizer->AddStretchSpacer();
    footerSizer->Add(btnSync = new wxButton(this, ID_MACHINE_FORM_BTN_SYNC, "sincronizar"));

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
    list->InsertColumn(0, "Item", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, "Valor", wxLIST_FORMAT_LEFT, 350);
    
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
    list->InsertColumn(0, "Nome", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, wxString::FromUTF8("Endereço MAC"), wxLIST_FORMAT_LEFT, 350);

    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->network_adapters.size(); j > 0; --j) {
        const sysinfo::network_adapter& adapter = machine->network_adapters[j - 1];

        i = list->InsertItem(0, adapter.name);
        list->SetItem(i, 1, adapter.mac);
    }
}

void inventory::MachineForm::setupDisks()
{
    list->InsertColumn(0, "Nome", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, wxString::FromUTF8("Número de Série"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(2, wxString::FromUTF8("Espaço"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(3, "Modelo", wxLIST_FORMAT_LEFT, 200);

    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->disks.size(); j > 0; --j) {
        const sysinfo::disk& disk = machine->disks[j - 1];

        i = list->InsertItem(0, disk.name);
        list->SetItem(i, 1, disk.seriaNumber);
        list->SetItem(i, 2, disk.size);
        list->SetItem(i, 3, disk.model);
    }
}

void inventory::MachineForm::setupMemory()
{
    list->InsertColumn(0, "Nome", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, wxString::FromUTF8("Capacidade"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(2, "Velocidade", wxLIST_FORMAT_LEFT, 200);

    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->physical_memories.size(); j > 0; --j) {
        const sysinfo::physical_memory& memory = machine->physical_memories[j - 1];

        i = list->InsertItem(0, memory.name);
        list->SetItem(i, 1, memory.capacity);
        list->SetItem(i, 2, memory.speed);
    }
}

void inventory::MachineForm::setupPrograms()
{
    list->InsertColumn(0, "Nome", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, wxString::FromUTF8("Versão"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(2, "Provedor", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(3, wxString::FromUTF8("Espaço Estimado"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(4, wxString::FromUTF8("Apenas no Usuário Atual"), wxLIST_FORMAT_LEFT, 200);

    long i;
    const sysinfo::machine *machine = model->GetMachine();

    for (size_t j = machine->programs.size(); j > 0; --j) {
        const sysinfo::program& program = machine->programs[j - 1];

        i = list->InsertItem(0, program.DisplayName);
        list->SetItem(i, 1, program.DisplayVersion);
        list->SetItem(i, 2, program.Publisher);
        list->SetItem(i, 3, program.EstimatedSize);
        list->SetItem(i, 4, program.CurrentUserOnly ? (wxString)"Sim" : wxString::FromUTF8("Não"));
    }
}

void inventory::MachineForm::setupAccounts()
{
    list->InsertColumn(0, "Nome", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, "Nome Completo", wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(2, wxString::FromUTF8("Domínio"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(3, wxString::FromUTF8("Descrição"), wxLIST_FORMAT_LEFT, 200);
    //list->InsertColumn(3, "Administrador", wxLIST_FORMAT_LEFT, 200);

    long i;
    const sysinfo::machine* machine = model->GetMachine();

    for (size_t j = machine->accounts.size(); j > 0; --j) {
        const sysinfo::user_account& account = machine->accounts[j - 1];

        i = list->InsertItem(0, account.name);
        list->SetItem(i, 1, account.fullName);
        list->SetItem(i, 2, account.domain);
        list->SetItem(i, 3, account.description);
    }
}

void inventory::MachineForm::OnBtnSync(wxCommandEvent& event)
{
    BlockChanges();
    model->UpdateExtraction();
}

void inventory::MachineForm::OnAppModelMachine(wxCommandEvent& event)
{
    BlockChanges(false);

    if (!event.GetInt())
        return;
        
    const machine_extraction* extraction = (const machine_extraction*)event.GetClientData();
    lastSync->SetLabel(extraction->datetime.Format(syncTextFormat));

    UpdateTreeSelection();
}