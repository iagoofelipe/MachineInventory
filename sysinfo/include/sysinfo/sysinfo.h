#ifndef SYSINFO_H
#define SYSINFO_H

#include "cJSON.h"

#include <string>
#include <vector>

namespace sysinfo
{
    // Flags
    enum Flag {
        IGNORE_SUBSOFTWARE = 1 << 0,
        CURRENT_USER = 1 << 1,
        LOCAL_MACHINE = 1 << 2,
        IGNORE_EMPTY_PROGRAM = 1 << 3,
        IGNORE_EMPTY_MAC = 1 << 4,
        IGNORE_EMPTY = IGNORE_EMPTY_PROGRAM | IGNORE_EMPTY_MAC,
        ALL_SOURCES = CURRENT_USER | LOCAL_MACHINE,
        DEFAULT = ALL_SOURCES | IGNORE_SUBSOFTWARE | IGNORE_EMPTY,
    };

    struct program {
        std::wstring DisplayName;
        std::wstring DisplayVersion;
        std::wstring Publisher;
        unsigned long EstimatedSize = 0;
        bool CurrentUserOnly = false;
    };

    struct disk {
        std::wstring name;
        std::wstring seriaNumber;
        std::wstring size;
        std::wstring model;
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
        std::wstring motherboardName;
        std::wstring motherboardManufacturer;
        std::wstring processorName;
        long processorClockSpeed = 0;
        std::vector<disk> disks;
        std::vector<network_adapter> network_adapters;
        std::vector<physical_memory> physical_memories;
        std::vector<program> programs;
    };

    bool Init();
    void Cleanup();
    std::wstring GetLastError();

    bool GetOperatingSystemName(std::wstring* out);
    bool GetDisks(std::vector<disk>* out);
    bool GetNetworkAdapters(std::vector<network_adapter>* out, int flags = DEFAULT);
    bool GetPhysicalMemories(std::vector<physical_memory>* out);
    bool GetPrograms(std::vector<program>* programs, int flags = DEFAULT);
    bool GetMotherboard(std::wstring* name, std::wstring* manufacturer);
    bool GetProcessor(std::wstring* name, long* clock_speed);
    bool GetMachine(machine* out, int flags = DEFAULT);

    cJSON* MachineToJson(const machine* data);
}

#endif