#ifndef SYNCFORM_H
#define SYNCFORM_H

#include "form.h"
#include "appmodel.h"

#include <map>
#include <wx/wx.h>

wxDECLARE_EVENT(EVT_SYNCFORM, wxCommandEvent);       // Sincronização requisitada

namespace inventory
{
    class SyncForm : public wxPanel, public IForm
    {
    public:
        enum STATE
        {
            GET_OWNER,
            READY
		};

        SyncForm(wxWindow* parent, int border = 0);

        // IForm
        void ShowMessage(const wxString& msg, int timeout = 5000) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

		void SetState(STATE state, bool force = false);

    private:
        wxCheckBox
            *m_checkbox;
        wxButton
            *m_btnBack,
            *m_btnContinue;
        wxStaticText
            *m_lbTitle,
            *m_lbMessage,
            * m_lbName,
            *m_lbMachineTitle;
        wxTextCtrl
            *m_txtCpf,
            *m_txtName,
            *m_txtMachineTitle;

		STATE
            m_state,
            m_prevState;
		AppModel* m_model;
		std::map<int, bool> m_changesBlocked;
		bool m_isBlocked;
    
        void setupUI(int border);
        void OnBtnContinue(wxCommandEvent& event);
        void OnCheckbox(wxCommandEvent& event);
    };
}

#endif