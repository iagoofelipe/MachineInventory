#pragma once

#pragma comment(lib, "wbemuuid.lib")

#include "cJSON.h"

#include <string>
#include <vector>
#include <map>

namespace sysinfo
{
    // Flags
    enum Flag {
        IGNORE_SUBSOFTWARE = 1 << 0,
        CURRENT_USER = 1 << 1,
        LOCAL_MACHINE = 1 << 2,
        IGNORE_EMPTY_PROGRAM = 1 << 3,
        IGNORE_EMPTY_MAC = 1 << 4,
        LOCAL_ACCOUNT_ONLY = 1 << 5,
        IGNORE_EMPTY = IGNORE_EMPTY_PROGRAM | IGNORE_EMPTY_MAC,
        ALL_SOURCES = CURRENT_USER | LOCAL_MACHINE,
        DEFAULT_FLAGS = ALL_SOURCES | IGNORE_SUBSOFTWARE | IGNORE_EMPTY,
    };

    struct program {
        std::wstring DisplayName;
        std::wstring DisplayVersion;
        std::wstring Publisher;
        std::wstring EstimatedSize;
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
        std::wstring speed;
    };

    struct user_group {
        std::wstring sid;
        std::wstring name;
        std::wstring description;
        std::wstring domain;
        std::wstring status;
        bool local;
    };

    struct user_account {
        std::wstring sid;
        std::wstring name;
        std::wstring fullName;
        std::wstring description;
        std::wstring domain;
        std::wstring status;
        bool disabled;
        bool local;
        bool lockout;
        bool passwordChangeable;
        bool passwordExpires;
        bool passwordRequired;
    };

    typedef std::map<std::wstring, std::vector<std::wstring>> user_accounts_by_group;
    typedef std::map<std::wstring, std::vector<std::wstring>> groups_by_user_account;

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
        std::wstring processorClockSpeed;
        std::vector<disk> disks;
        std::vector<network_adapter> network_adapters;
        std::vector<physical_memory> physical_memories;
        std::vector<program> programs;
        std::vector<user_group> groups;
        std::vector<user_account> accounts;
        user_accounts_by_group group_members;
    };

    bool Init();
    void Cleanup();
    std::wstring GetLastError();

    bool GetDisks(std::vector<disk>* out);
    bool GetNetworkAdapters(std::vector<network_adapter>* out, int flags = 0);
    bool GetPhysicalMemories(std::vector<physical_memory>* out);
    bool GetPrograms(std::vector<program>* programs, int flags = 0);
    bool GetMotherboard(std::wstring* name, std::wstring* manufacturer);
    bool GetProcessor(std::wstring* name, std::wstring* clock_speed);
    bool GetMachine(machine* out, int flags = DEFAULT_FLAGS);
    bool GetUserAccounts(std::vector<user_account>* out, int flags = 0);
    bool GetUserAccountGroups(user_accounts_by_group* map_group_key, user_accounts_by_group* map_account_key, int flags = 0);
    bool GetGroups(std::vector<user_group>* out, int flags = 0);

    cJSON* MachineToJson(const machine* data);
    bool MachineFromJString(const char* str_data, machine* out);
}