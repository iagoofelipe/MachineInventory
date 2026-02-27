#ifndef STRUCTS_H
#define STRUCTS_H

#include <wx/wx.h>

namespace inventory
{
    struct auth
    {
        wxString cpf;
        wxString password;
    };

    struct newUser
    {
        wxString cpf;
        wxString name;
        wxString password;
    };
}

#endif