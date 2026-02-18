#pragma once

#include <wx/wx.h>
#include <server.h>

class MachineInventoryApp :
    public wxApp
{
	virtual bool OnInit() override;
	virtual int OnExit() override;

private:
	sysinfo::ServerConnection* pServer = nullptr;
	sysinfo::machine* pMachine = nullptr;
};

