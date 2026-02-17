#include "console.h"
#include "server.h"
#include "utils.h"

#include <iostream>

static bool machine_data_required;
static sysinfo::machine machine;
static cJSON *json_machine;

static std::wstring input(const wchar_t* msg = NULL, const wchar_t* def = NULL)
{
    if (def)
        return def;

    std::wstring buffer;
    std::wcout << (msg? msg : L"");
    std::getline(std::wcin, buffer);
    
    return buffer;
}

static std::string input(const char* msg = NULL, const wchar_t* def = NULL)
{
    if (def)
        return ToString(def);

    std::wstring buffer;
    std::wcout << (msg? msg : "");
    std::getline(std::wcin, buffer);
    
    return ToString(buffer);
}

void show_help()
{
    std::wcerr
        << "Usage: console.exe help | sysinfo | validatetoken | auth [CPF [PASSWORD]] | upload [CPF [TITLE]] | newuser [CPF [NAME [PASSWORD]]\n"
        << "commands:\n"
        << "\thelp\tShow this message\n"
        << "\tsysinfo\tShow the system information\n"
		<< "\tvalidatetoken\tValidate the current token\n"
        << "\tupload\tGet the system information and upload with the user's CPF\n"
        << "\tnewuser\tCreate a new user\n";
}

bool init_console(const std::wstring_view& command)
{
    configure_terminal();

    machine_data_required = command == L"sysinfo" || command == L"upload";
    json_machine = NULL;

    if (!machine_data_required)
        return true;

    if (!sysinfo::init()) {
        std::wcerr << sysinfo::get_last_error() << std::endl;
        return false;
    }

    std::wcout << "getting the machine data..." << std::endl;

    if (!sysinfo::get_machine(&machine)) {
        std::wcerr << sysinfo::get_last_error() << std::endl;
        sysinfo::cleanup();
        return false;
    }

    if (!(json_machine = sysinfo::machine_to_cjson(&machine))) {
        std::wcerr << sysinfo::get_last_error() << std::endl;
        sysinfo::cleanup();
        return false;
    }

    return true;
}

void cleanup()
{
    if (!machine_data_required)
        return;

    if (json_machine) cJSON_Delete(json_machine);
    sysinfo::cleanup();
}

int command_sysinfo()
{
    std::wcout << cJSON_Print(json_machine) << std::endl;
    return 0;
}

int command_upload(const wchar_t* cpf, const wchar_t* machineTitle)
{
    sysinfo::ServerConnection server;
    std::string userId, newMachineId;

    if (
        !server.is_ready() ||
		!server.validate_token() ||
        !server.upload_machine(
            json_machine,
            input("User's CPF: ", cpf).c_str(),
            input("Machine's Title: ", machineTitle).c_str(),
            &newMachineId
        )
    ) {
        std::wcerr << server.get_last_error() << std::endl;
        return 1;
    }

    std::wcout << "current version of the machine uploaded successfully, MachineID '" << newMachineId.c_str() << "'\n";
    return 0;
}

int command_auth(const wchar_t* cpf, const wchar_t* password)
{
    sysinfo::ServerConnection server;

    if (
        !server.is_ready() ||
        !server.login(input("User's CPF: ", cpf), input("User's password: ", password))
    ) {
        std::wcerr << server.get_last_error() << std::endl;
        return 1;
	}

    std::wcout << "Token generated with success\n";
    return 0;
}

int command_newuser(const wchar_t* cpf, const wchar_t* name, const wchar_t* password)
{
    sysinfo::ServerConnection server;
    std::string userId;

    if (
        !server.is_ready() ||
        !server.create_new_user(input("User's CPF: ", cpf), input("User's name: ", name), input("User's password: ", password), &userId)
    ) {
        std::wcerr << server.get_last_error() << std::endl;
        return 1;
    }

    std::wcout << "user created with ID '" << userId.c_str() << "'\n";
    return 0;
}

int command_validatetoken()
{
	sysinfo::ServerConnection server;
    if (!server.is_ready() || !server.validate_token())
    {
		std::wcerr << server.get_last_error() << std::endl;
        return 1;
    }

    std::wcout << "token valid\n";
    return 0;
}
