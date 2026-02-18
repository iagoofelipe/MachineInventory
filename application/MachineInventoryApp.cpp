#include "MachineInventoryApp.h"
#include "MainWindow.h"

#include <sysinfo.h>


wxIMPLEMENT_APP(MachineInventoryApp);

bool MachineInventoryApp::OnInit()
{
	pMachine = new sysinfo::machine();

	if (
		!sysinfo::init() ||
		!sysinfo::get_machine(pMachine)
		) {
		wxMessageBox(L"Failed: " + sysinfo::get_last_error(), "Init Error", wxOK | wxICON_ERROR);
		return false;
	}

	pServer = new sysinfo::ServerConnection();

	if (!pServer->is_ready()) {
		wxMessageBox((wxString)"Failed: " + pServer->get_last_error(), "Server Error", wxOK | wxICON_ERROR);
		return false;
	}

	MainWindow* win = new MainWindow(pServer, pMachine);
	return win->Show();
}

int MachineInventoryApp::OnExit()
{
	delete pServer;
	delete pMachine;
	sysinfo::cleanup();
	//wxMessageBox("Clean up finished successfully", "App Exit", wxOK | wxICON_INFORMATION);
	return wxApp::OnExit();
}