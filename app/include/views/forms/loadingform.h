#ifndef LOADINGFORM_H
#define LOADINGFORM_H

#include "form.h"

#include <wx/wx.h>

namespace inventory
{
    class LoadingForm : public wxPanel, public IForm
    {
    public:
        LoadingForm(wxWindow* parent, const wxString& msg = "inicializando componentes...");

        // IForm
        void ShowMessage(const wxString& msg) override;
        void BlockChanges(bool block = true) override;
        void Clear() override;

    private:
        wxStaticText* lbMessage;
    };
}

#endif