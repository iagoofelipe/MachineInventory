#pragma once

#include <string>
#include <vector>

namespace sysinfo
{
    // Flags
    enum Flag {
        IGNORE_SUBSOFTWARE = 1 << 0,
        CURRENT_USER = 1 << 1,
        LOCAL_MACHINE = 1 << 2,
        IGNORE_EMPTY = 1 << 3,
        ALL_SOURCES = CURRENT_USER | LOCAL_MACHINE,
        DEFAULT = ALL_SOURCES | IGNORE_SUBSOFTWARE | IGNORE_EMPTY,
    };

    struct program {
        std::wstring DisplayName;
        std::wstring DisplayVersion;
        std::wstring Publisher;
        unsigned long EstimatedSize;
        bool CurrentUserOnly;
    };

    struct logical_disk {
        std::wstring freeSpace;
        std::wstring size;
        std::wstring unit;
    };

    struct network_adapter {
        std::wstring name;
        std::wstring mac;
    };

    struct physical_memory {
        std::wstring name;
        std::wstring capacity;
        long speed;
        std::wstring capaticyUnit;
        std::wstring speedUnit;
    };

    struct machine {
        std::wstring osName;
        std::wstring osArchitecture;
        std::wstring osInstallDate;
        std::wstring osVersion;
        std::wstring osOrganization;
        std::wstring osSerialNumber;
        std::wstring motherboardManufacturer;
        std::wstring processor;
        std::vector<logical_disk> logical_disks;
        std::vector<network_adapter> network_adapters;
        std::vector<physical_memory> physical_memories;
        std::vector<program> programs;
    };

    bool init();
    void cleanup();
    std::wstring get_last_error();

    bool get_operating_system_name(std::wstring* out);
    bool get_logical_disks(std::vector<logical_disk>* out);
    bool get_network_adapters(std::vector<network_adapter>* out);
    bool get_physical_memories(std::vector<physical_memory>* out);
    bool get_programs(std::vector<program>* programs, int flags = DEFAULT);
    bool get_motherboard_manufacturer(std::wstring* out);
    bool get_processor_name(std::wstring* out);
    bool get_machine(machine* out);
}