#include <iostream>
#include <string_view>

#include "sysinfo.h"
#include "console.h"

int wmain(int argc, wchar_t** argv)
{
	std::wstring_view command = (argc > 1) ? argv[1] : L"help";

    if (command == L"help") {
        std::wcerr << "Usage: " << argv[0] << " sysinfo | help | upload [CPF [TITLE]]\n"
            << "commands:\n"
			<< "\tsysinfo\tShow the system information\n"
            << "\tupload\tGet the system information and upload with the user's CPF\n"
            << "\thelp\tShow this message\n";
        return 1;
    }


    if (!init_console())
        return 1;


    if (!sysinfo::init()) {
        std::wcerr << "[sysinfo::init ERROR] " << sysinfo::get_last_error() << std::endl;
		cleanup_console();
        return 1;
    }

    sysinfo::machine machine;

    std::wcout << "getting the machine data..." << std::endl;

    if (!sysinfo::get_machine(&machine)) {
        std::wcerr << "[sysinfo::get_machine ERROR] " << sysinfo::get_last_error() << std::endl;
        cleanup_console();
        return 1;
    }

    if (command == L"sysinfo")
        show_machine(machine);

    else if (command == L"upload") {
        bool success = upload_machine(
            machine,
            argc > 2 ? argv[2] : NULL,
			argc > 3 ? argv[3] : NULL
        );

        if (success)
            std::wcout << "Machine information uploaded successfully!" << std::endl;
    }

    sysinfo::cleanup();
    cleanup_console();

    return 0;
}