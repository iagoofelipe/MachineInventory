#include "MainWindow.h"

#include <wx/splitter.h>
#include <wx/treectrl.h>

MainWindow::MainWindow(sysinfo::ServerConnection* server, sysinfo::machine* machine)
	: wxFrame(nullptr, wxID_ANY, "Machine Inventory")
	, list(nullptr)
	, server(server)
	, machine(machine)
	, authOwnerForm(nullptr)
{
	setupUI();
	sysinfo::user owner;

	if (
		!server->has_token() ||
		!server->get_user(&owner)
	) {
		authOwnerForm->setState(AUTH);
		return;
	}

	

	authOwnerForm->setCpf(owner.cpf);
	authOwnerForm->setName(owner.name);
	authOwnerForm->setState(MACHINE_OWNER);
}

void MainWindow::setupUI()
{
	wxFont font = GetFont();
	font.SetPointSize(11);
	SetFont(font);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	authOwnerForm = new AuthOwnerForm(this, server);
	
	sizer->Add(authOwnerForm, 1, wxEXPAND | wxALL, 10);

	SetSizer(sizer);




	//wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);

	//wxTreeCtrl* tree = new wxTreeCtrl(splitter, wxID_ANY);
	//wxTreeItemId root = tree->AddRoot("Resumo do Sistema");

	//wxTreeItemId comp = tree->AppendItem(root, "Componentes");
	//tree->AppendItem(comp, "Multimídia");
	//tree->AppendItem(comp, "Rede");
	//
	//wxTreeItemId env = tree->AppendItem(root, "Ambiente de Software");
	//tree->AppendItem(env, "Variáveis de Ambiente");

	//tree->Expand(root);

	//list = new wxListCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
	//
	//wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	//sizer->Add(splitter, 1, wxEXPAND | wxALL, 10);
	//
	//splitter->SplitVertically(tree, list, 250);

	//SetSizer(sizer);
	//Layout();

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	SetClientSize(400, 500);
	Center();

	//setupResumeInfo();
}

void MainWindow::setupResumeInfo()
{
	long i;
	
	list->Freeze();
	list->DeleteAllColumns();
	list->DeleteAllItems();

	// Columns
	list->InsertColumn(0, wxT("Item"), wxLIST_FORMAT_LEFT, 200);
	list->InsertColumn(1, wxT("Valor"), wxLIST_FORMAT_LEFT, 400);

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

	list->Thaw();
}
