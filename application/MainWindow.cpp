#include "MainWindow.h"

#include <wx/splitter.h>
#include <wx/treectrl.h>

MainWindow::MainWindow()
	: wxFrame(nullptr, wxID_ANY, "Machine Inventory")
	, init_success(false)
	, list(nullptr)
	, machine()
{
	if (!(init_success = sysinfo::init() && sysinfo::get_machine(&machine))) {
		return;
	}

	setupUI();
}

bool MainWindow::Show()
{
	if (!init_success) {
		wxMessageBox(L"Failed: " + sysinfo::get_last_error(), "Error", wxOK | wxICON_ERROR);
		return false;
	}

	return wxFrame::Show();
}

void MainWindow::setupUI()
{
	wxFont font = GetFont();
	font.SetPointSize(10);
	SetFont(font);

	wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);

	wxTreeCtrl* tree = new wxTreeCtrl(splitter, wxID_ANY);
	wxTreeItemId root = tree->AddRoot("Resumo do Sistema");

	wxTreeItemId comp = tree->AppendItem(root, "Componentes");
	tree->AppendItem(comp, "Multimídia");
	tree->AppendItem(comp, "Rede");
	
	wxTreeItemId env = tree->AppendItem(root, "Ambiente de Software");
	tree->AppendItem(env, "Variáveis de Ambiente");

	tree->Expand(root);

	list = new wxListCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
	
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(splitter, 1, wxEXPAND | wxALL, 10);
	
	splitter->SplitVertically(tree, list, 250);

	SetSizer(sizer);
	Layout();

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	SetClientSize(900, 600);
	Center();

	setupResumeInfo();
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
	list->SetItem(i, 1, machine.processor);

	i = list->InsertItem(0, L"Organização");
	list->SetItem(i, 1, machine.osOrganization);

	i = list->InsertItem(0, L"Fabricante");
	list->SetItem(i, 1, machine.motherboardManufacturer);

	i = list->InsertItem(0, L"Número de Série");
	list->SetItem(i, 1, machine.osSerialNumber);

	i = list->InsertItem(0, L"Data de Instalação");
	list->SetItem(i, 1, machine.osInstallDate);

	i = list->InsertItem(0, L"Arquitetura");
	list->SetItem(i, 1, machine.osArchitecture);

	i = list->InsertItem(0, L"Versão do Sistema");
	list->SetItem(i, 1, machine.osVersion);

	i = list->InsertItem(0, L"Sistema Operacional");
	list->SetItem(i, 1, machine.osName);

	list->Thaw();
}
