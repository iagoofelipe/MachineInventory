#pragma once

#include "AuthOwnerForm.h"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <sysinfo.h>
#include <server.h>

class MainWindow :
    public wxFrame
{
public:
    MainWindow(sysinfo::ServerConnection* server, sysinfo::machine* machine);

private:
	wxListCtrl* list;
	sysinfo::ServerConnection* server;
	sysinfo::machine* machine;
	AuthOwnerForm* authOwnerForm;

	void setupUI();
	void setupResumeInfo();
};

