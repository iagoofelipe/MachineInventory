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
        HomeForm(wxWindow* parent, SyncForm* syncForm = nullptr, MachineForm* machineForm = nullptr);

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

        bool GetSyncFormGenerated() const { return syncFormGenerated; }
        bool GetMachineFormGenerated() const { return machineFormGenerated; }

    private:
        SyncForm* syncForm;
        MachineForm* machineForm;
        wxNotebook* notebook;
        bool syncFormGenerated;
        bool machineFormGenerated;

        void setupUI();
        wxPanel* addPage(wxWindow* page, const wxString& title); 
    };
}

#endif