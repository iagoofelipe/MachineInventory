#ifndef HOMEFORM_H
#define HOMEFORM_H

#include "form.h"
#include "syncform.h"
#include "machineform.h"

#include <wx/wx.h>
#include <wx/notebook.h>

namespace inventory
{
    class HomeForm : public wxPanel, public IForm
    {
    public:
        HomeForm(wxWindow* parent, wxWindowID id);

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

        SyncForm* GetSyncForm() const { return syncForm; }
        MachineForm* GetMachineForm() const { return machineForm; }

    private:
        SyncForm* syncForm;
        MachineForm* machineForm;
        wxNotebook* notebook;

        void setupUI();
        // wxPanel* addPage(wxWindow* page, const wxString& title); 
    };
}

#endif