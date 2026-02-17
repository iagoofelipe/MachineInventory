#include <iostream>
#include <string_view>

#include "console.h"
#include "sysinfo.h"
#include "utils.h"

int wmain(int argc, wchar_t** argv)
{
	std::wstring_view command = (argc > 1) ? argv[1] : L"help";

    if (!init_console(command))
        return 1;
    
    // considerando PROGRAM e COMMAND
    int code = 0;
    const wchar_t* arg1 = argc > 2 ? argv[2] : NULL;
    const wchar_t* arg2 = argc > 3 ? argv[3] : NULL;
    const wchar_t* arg3 = argc > 4 ? argv[4] : NULL;

    if (command == L"sysinfo") code = command_sysinfo();
    else if (command == L"upload") code = command_upload(arg1, arg2);
    else if (command == L"newuser") code = command_newuser(arg1, arg2, arg3);
    else if (command == L"auth") code = command_auth(arg1, arg2);
    else if (command == L"validatetoken") code = command_validatetoken();
    else show_help();


    cleanup();
    return code;
}