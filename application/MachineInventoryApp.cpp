#include "MachineInventoryApp.h"
#include "MainWindow.h"

wxIMPLEMENT_APP(MachineInventoryApp);

bool MachineInventoryApp::OnInit()
{
	MainWindow* win = new MainWindow();
	return win->Show();
}
