#include "console.h"

#include <fcntl.h>  // Necessario para _O_U16TEXT
#include <io.h>     // Necessario para _setmode

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
    _setmode(_fileno(stdin), _O_WTEXT);    // configura UTF-16 para std::wcin
    _setmode(_fileno(stdout), _O_U16TEXT); // configura UTF-16 para std::wcout

    machine_data_required = command == L"sysinfo" || command == L"upload";
    json_machine = NULL;

    if (!machine_data_required)
        return true;

    if (!sysinfo::Init()) {
        std::wcerr << sysinfo::GetLastError() << std::endl;
        return false;
    }

    std::wcout << "getting the machine data..." << std::endl;

    if (!sysinfo::GetMachine(&machine) || !(json_machine = sysinfo::MachineToJson(&machine))) {
        std::wcerr << sysinfo::GetLastError() << std::endl;
        sysinfo::Cleanup();
        return false;
    }

    return true;
}

void cleanup()
{
    if (!machine_data_required)
        return;

    if (json_machine) cJSON_Delete(json_machine);
    sysinfo::Cleanup();
}

int command_sysinfo()
{
    std::wcout
        << "Operating System: '" << machine.osName << "'\n" 
        << "Architecture: '" << machine.osArchitecture << "'\n"
        << "Install Date: '" << machine.osInstallDate << "'\n"
        << "Version: '" << machine.osVersion << "'\n"
        << "Serial Number: '" << machine.osSerialNumber << "'\n"
        << "Organization: '" << machine.osOrganization << "'\n"
        << "Motherboard: '" << machine.motherboardName << "'\n"
        << "Motherboard Manufacturer: '" << machine.motherboardManufacturer << "'\n"
        << "Processor: '" << machine.processorName << "'\n"
        << "Processor Clock Speed: " << machine.processorClockSpeed / 1000 << " GHz\n"
        << "Disks ("<< machine.disks.size() << "):\n";
        
    for (const sysinfo::disk& disk : machine.disks)
        std::wcout
            << "\tDisk(Name='" << disk.name << "' "
            << "SerialNumber='" << disk.seriaNumber << "' "
            << "Size=" << disk.size << " "
            << "Model='" << disk.model << "')\n";

    std::wcout << "Metwork Adapters ("<< machine.network_adapters.size() << "):\n";
    for (const sysinfo::network_adapter& adapter : machine.network_adapters)
        std::wcout
            << "\tNetworkAdapter(Name='" << adapter.name << "' "
            << "Mac='" << adapter.mac << "')\n";

    std::wcout << "Physical Memories ("<< machine.physical_memories.size() << "):\n";
    for (const sysinfo::physical_memory& memory : machine.physical_memories)
        std::wcout
            << "\tPhysicalMemory((Name='" << memory.name << "' "
            << "Capacity=" << memory.capacity << " "
            << "Speed=" << memory.speed << ")\n";

    std::wcout << "Programs ("<< machine.programs.size() << "):\n";
    for (const sysinfo::program& program : machine.programs)
        std::wcout
            << "\tProgram(Name='" << program.DisplayName << "' "
            << "Version='" << program.DisplayVersion << "' "
            << "Publisher='" << program.Publisher << "' "
            << "EstimatedSize=" << program.EstimatedSize << " "
            << "CurrentUserOnly=" << (program.CurrentUserOnly? "True" : "False") << ")\n";

    return 0;
}

int command_upload(const wchar_t* cpf, const wchar_t* machineTitle)
{
    sysinfo::ServerAPI server;
    std::string userId, newMachineId;

    if (
        !server.Initialize() ||
		!server.ValidateToken() ||
        !server.UploadMachine(
            json_machine,
            input("User's CPF: ", cpf).c_str(),
            input("Machine's Title: ", machineTitle).c_str(),
            &newMachineId
        )
    ) {
        std::wcerr << server.GetLastError() << std::endl;
        return 1;
    }

    std::wcout << "current version of the machine uploaded successfully, MachineID '" << newMachineId.c_str() << "'\n";
    return 0;
}

int command_auth(const wchar_t* cpf, const wchar_t* password)
{
    sysinfo::ServerAPI server;

    if (
        !server.Initialize() ||
        !server.Auth(input("User's CPF: ", cpf), input("User's password: ", password))
    ) {
        std::wcerr << server.GetLastError() << std::endl;
        return 1;
	}

    std::wcout << "Token generated with success\n";
    return 0;
}

int command_newuser(const wchar_t* cpf, const wchar_t* name, const wchar_t* password)
{
    sysinfo::ServerAPI server;
    std::string userId;

    if (
        !server.Initialize() ||
        !server.CreateNewUser(input("User's CPF: ", cpf), input("User's name: ", name), input("User's password: ", password), &userId)
    ) {
        std::wcerr << server.GetLastError() << std::endl;
        return 1;
    }

    std::wcout << "user created with ID '" << userId.c_str() << "'\n";
    return 0;
}

int command_validatetoken()
{
	sysinfo::ServerAPI server;
    if (!server.Initialize() || !server.ValidateToken())
    {
		std::wcerr << server.GetLastError() << std::endl;
        return 1;
    }

    std::wcout << "token valid\n";
    return 0;
}
