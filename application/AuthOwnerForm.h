#pragma once

#include <wx/wx.h>
#include <server.h>

enum AuthOwnerFormState
{
	NONE,
	AUTH,
	NEW_USER,
	MACHINE_OWNER,
	READY_TO_SYNC,
};

class AuthOwnerForm :
    public wxPanel
{
public:
	AuthOwnerForm(wxWindow* parent, sysinfo::ServerAPI* server, AuthOwnerFormState initial_state = NONE);

	void setState(AuthOwnerFormState state);
	void setName(const wxString& text);
	void setCpf(const wxString& text);

private:
	AuthOwnerFormState currentState;
	AuthOwnerFormState secondaryState;
	sysinfo::ServerAPI* server;

	wxStaticText* lbTitle;
	wxCheckBox* checkbox;
	wxStaticText* lbCpf;
	wxTextCtrl* txtCpf;
	wxStaticText* lbPass;
	wxTextCtrl* txtPass;
	wxStaticText* lbPassConfirm;
	wxTextCtrl* txtPassConfirm;
	wxStaticText* lbName;
	wxTextCtrl* txtName;
	wxStaticText* lbMessage;
	wxButton* btnSecondary;
	wxButton* btnContinue;

	void setupUI();
	void on_checkbox(wxCommandEvent& event);
	void on_btnSecondary(wxCommandEvent& event);
	void on_btnContinue(wxCommandEvent& event);
};

