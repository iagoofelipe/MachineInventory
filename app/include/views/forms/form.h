#ifndef FORM_H
#define FORM_H

#include <wx/wx.h>

namespace inventory
{
    class IForm
    {
    public:
        virtual ~IForm() {}
        virtual void ShowMessage(const wxString& msg) = 0;
        virtual void BlockChanges(bool block = true) = 0;
        virtual void Clear() = 0;
    };
}

#endif