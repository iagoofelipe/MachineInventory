#ifndef SYNCFORM_H
#define SYNCFORM_H

#include "form.h"

#include <wx/wx.h>

namespace inventory
{
    class SyncForm : public wxPanel, public IForm
    {
    public:
        SyncForm(wxWindow* parent);

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

    private:
        wxStaticText
            *lbTitle,
            *lbMessage,
            *lbMachineTitle;
        wxTextCtrl
            *txtCpf,
            *txtName,
            *txtMachineTitle;
    
        void setupUI();
        void OnBtnBack(wxCommandEvent& event);
        void OnBtnContinue(wxCommandEvent& event);
        void OnCheckbox(wxCommandEvent& event);
    };
}

#endif