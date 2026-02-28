#ifndef MACHINEFORM_H
#define MACHINEFORM_H

#include "form.h"
#include "appmodel.h"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>

namespace inventory
{
    class MachineForm : public wxPanel, public IForm
    {
    public:
        MachineForm(wxWindow* parent, wxWindowID id, int border = 0);

        enum ID
        {
            ID_BTN_SYNC = wxID_HIGHEST + 1,
            ID_TREE_MACHINE,
        };

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

        void UpdateTreeSelection();
        void UpdateTreeSelection(wxTreeItemId id);

    private:
        AppModel* model;
        wxListCtrl* list;
        wxTreeCtrl* tree;
        wxStaticText* lastSync;
        wxButton* btnSync;
        
        void setupUI(int border);
        void setupSummary();
        void setupNewtwork();
        void setupDisks();
        void setupMemory();
        void setupPrograms();

        // void OnTreeSelChange(wxTreeEvent& event);
        void OnBtnSync(wxCommandEvent& event);
        void OnAppModelMachine(wxCommandEvent& event);
    };
}

#endif