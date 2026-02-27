#ifndef FORMS_AUTH_H
#define FORMS_AUTH_H

#include "form.h"
#include "models/appmodel.h"
#include "models/structs.h"

#include <wx/wx.h>

wxDECLARE_EVENT(EVT_AUTHFORM_AUTH, wxCommandEvent);
wxDECLARE_EVENT(EVT_AUTHFORM_CREATEACCOUNT, wxCommandEvent);
wxDECLARE_EVENT(EVT_AUTHFORM_MACHINE, wxCommandEvent);

namespace inventory
{
    class AuthForm : public wxPanel, public IForm
    {
    public:
        enum State
        {
            NONE,
            AUTH,
            CREATE_ACCOUNT
        };

        AuthForm(wxWindow* parent, State state = AUTH);
        void SetState(State state);

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

    private:
        State currentState;
        AppModel& model;
        auth authData;
        newUser newUserData;

        wxStaticText
            *lbTitle,
            *lbCpf,
            *lbName,
            *lbPass,
            *lbPassConfirm,
            *lbMessage;
        wxTextCtrl
            *txtCpf,
            *txtName,
            *txtPass,
            *txtPassConfirm;
        wxButton
            *btnAccess,
            *btnCreateAccount,
            *btnBack,
            *btnMachine;

        void setupUI();

        // Events
        void OnBtnAccess(wxCommandEvent& event);
        void OnBtnCreateAccount(wxCommandEvent& event);
        void OnBtnBack(wxCommandEvent& event);
        void OnBtnMachine(wxCommandEvent& event);
    };
}

#endif