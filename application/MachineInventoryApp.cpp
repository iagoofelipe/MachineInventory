#include "MachineInventoryApp.h"
#include "MainWindow.h"

#include <sysinfo.h>


wxIMPLEMENT_APP(MachineInventoryApp);

bool MachineInventoryApp::OnInit()
{
	pMachine = new sysinfo::machine();

	if (
		!sysinfo::Init() ||
		!sysinfo::GetMachine(pMachine)
		) {
		wxMessageBox(L"Failed: " + sysinfo::GetLastError(), "Init Error", wxOK | wxICON_ERROR);
		return false;
	}

	pServer = new sysinfo::ServerAPI();

	if (!pServer->Initialize()) {
		wxMessageBox((wxString)"Failed: " + pServer->GetLastError(), "Server Error", wxOK | wxICON_ERROR);
		return false;
	}

	MainWindow* win = new MainWindow(pServer, pMachine);
	return win->Show();
}

int MachineInventoryApp::OnExit()
{
	delete pServer;
	delete pMachine;
	sysinfo::Cleanup();
	//wxMessageBox("Clean up finished successfully", "App Exit", wxOK | wxICON_INFORMATION);
	return wxApp::OnExit();
}