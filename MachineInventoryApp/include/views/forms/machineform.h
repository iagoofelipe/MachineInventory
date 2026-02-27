#ifndef MACHINEFORM_H
#define MACHINEFORM_H

#include "views/forms/form.h"
#include "models/appmodel.h"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>

namespace inventory
{
    class MachineForm : public wxPanel, public IForm
    {
    public:
        MachineForm(wxWindow* parent);

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

    private:
        AppModel& model;
        wxListCtrl* list;
        wxTreeCtrl* tree;
        
        void setupUI();
        void setupSummary();

        void OnTreeSelChange(wxTreeEvent& event);
    };
}

#endif