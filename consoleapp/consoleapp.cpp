// consoleapp.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include "sysinfo.h"
#include "utils.h"

int main()
{
    sysinfo::machine machine;
    int i = 0;

    configure_terminal();

    if (!sysinfo::init()) {
        std::wcerr << "[sysinfo::init ERROR] " << sysinfo::get_last_error() << std::endl;
        return 1;
    }

    if (!sysinfo::get_machine(&machine)) {
        std::wcerr << "[sysinfo::get_machine ERROR] " << sysinfo::get_last_error() << std::endl;
        return 1;
    }

    std::wcout
        << "Operating System: " << machine.osName << std::endl
        << "Architecture: " << machine.osArchitecture << std::endl
        << "Install Date: " << machine.osInstallDate << std::endl
        << "Version: " << machine.osVersion << std::endl
        << "Organization: " << machine.osOrganization << std::endl
        << "Serial Number: " << machine.osSerialNumber << std::endl
        << "Vendor: " << machine.motherboardManufacturer << std::endl
        << "Processor: " << machine.processor << std::endl;

    i = 0;
    std::wcout << "\nDisks:\n";
    for (const sysinfo::disk& disk : machine.disks) {
        std::wcout
            << "\t(" << ++i << ")"
            << " Name='" << disk.name << "'"
            << " SerialNumber='" << disk.seriaNumber << "'"
            << " Sizze='" << disk.size << "'"
            << " Model='" << disk.model << "'"
            << std::endl;
    }

    i = 0;
    std::wcout << "\nNetwork Adapters:\n";
    for (const sysinfo::network_adapter& adapter : machine.network_adapters) {
        std::wcout
            << "\t(" << ++i << ")"
            << " Name='" << adapter.name << "'"
            << " Mac='" << adapter.mac << "'"
            << std::endl;
    }

    i = 0;
    std::wcout << "\nPhysical Memories:\n";
    for (const sysinfo::physical_memory& memory : machine.physical_memories) {
        std::wcout
            << "\t(" << ++i << ")"
            << " Name='" << memory.name << "'"
            << " FreeSpace='" << memory.capacity << " " << memory.capaticyUnit << "'"
            << " Speed='" << memory.speed << " " << memory.speedUnit << "'"
            << std::endl;
    }

    i = 0;
    std::wcout << "\nPrograms:\n";
    for (const sysinfo::program& p : machine.programs) {
        std::wcout
            << "\t(" << ++i << ")"
            << " DisplayName='" << p.DisplayName << "'"
            << " DisplayVersion='" << p.DisplayVersion << "'"
            << " Publisher='" << p.Publisher << "'"
            << " EstimatedSize='" << p.EstimatedSize << " KB'"
            << " CurrentUserOnly=" << (p.CurrentUserOnly ? "True" : "False")
            << std::endl;
    }

    sysinfo::cleanup();

    return 0;
}