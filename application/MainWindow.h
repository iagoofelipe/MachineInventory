#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <sysinfo.h>

class MainWindow :
    public wxFrame
{
public:
    MainWindow();

	bool Show();

private:
    bool init_success;
	wxListCtrl* list;
	sysinfo::machine machine;

	void setupUI();
	void setupResumeInfo();
};

