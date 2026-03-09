#include "sysinfo.h"
#include "utils.h"

#include <sstream>
#include <iostream>
#include <windows.h>
#include <wbemidl.h>

static IWbemServices* pSvc = NULL;
static IWbemLocator* pLoc = NULL;
static bool coInitialized = false;
static std::wstring _error;

static const std::vector<LPCWSTR> FIELDS_LOGICAL_DISK{ L"FreeSpace", L"Size" };
static const std::vector<LPCWSTR> FIELDS_DISK_DRIVE{ L"Caption", L"SerialNumber", L"Size", L"Model" };
static const std::vector<LPCWSTR> FIELDS_NETWORK_ADPATER{ L"Name", L"MACAddress" };
static const std::vector<LPCWSTR> FIELDS_PHYSICAL_MEMORY{ L"Tag", L"Capacity", L"Speed" };
static const std::vector<LPCWSTR> FIELDS_PROCESSOR{ L"Name", L"CurrentClockSpeed" };
static const std::vector<LPCWSTR> FIELDS_OPERATING_SYSTEM{ L"Caption", L"Version", L"InstallDate", L"OSArchitecture", L"SerialNumber", L"Organization" };
static const std::vector<LPCWSTR> FIELDS_BASE_BOARD{ L"Manufacturer", L"Product" };
static const std::vector<LPCWSTR> FIELDS_USER_ACCOUNT{
    L"SID", L"Name", L"FullName", L"Description", L"Domain", L"Status", L"Disabled", L"LocalAccount",
    L"Lockout", L"PasswordChangeable", L"PasswordExpires", L"PasswordRequired"
};
static const std::vector<LPCWSTR> FIELDS_GROUP{ L"SID", L"Name", L"Description", L"Domain", L"Status", L"LocalAccount" };
static const std::vector<LPCWSTR> FIELDS_GROUP_USER{ L"GroupComponent", L"PartComponent" };


static bool get_program(HKEY root, LPCWSTR subkey, sysinfo::program* p, int flags, bool currentUser)
{
    HKEY hKey;
    LSTATUS status;

    if (RegOpenKeyExW(root, subkey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return false;

    // variaveis para consulta
    DWORD dwType;
    WCHAR wValue[255];
    DWORD wSize;
    DWORD dwValue;
    DWORD dwSize;

    // caso seja um componente de software e IGNORE_SUBSOFTWARE esteja definido
    if ((flags & sysinfo::IGNORE_SUBSOFTWARE) && (RegQueryValueExW(hKey, L"ParentKeyName", NULL, NULL, NULL, NULL) == ERROR_SUCCESS)) {
        RegCloseKey(hKey);
        return false;
    }

    // DisplayName
    wSize = sizeof(wValue);
    status = RegQueryValueExW(hKey, L"DisplayName", NULL, &dwType, (LPBYTE)wValue, &wSize);

    // caso DisplayName venha em branco e IGNORE_EMPTY esteja definido
    if (flags & sysinfo::IGNORE_EMPTY_PROGRAM && (dwType == VT_EMPTY || status != ERROR_SUCCESS)) {
        RegCloseKey(hKey);
        return false;
    }

    p->DisplayName = status == ERROR_SUCCESS && dwType == REG_SZ ? wValue : L"";


    // DisplayVersion
    wSize = sizeof(wValue);
    status = RegQueryValueExW(hKey, L"DisplayVersion", NULL, &dwType, (LPBYTE)wValue, &wSize);
    p->DisplayVersion = status == ERROR_SUCCESS && dwType == REG_SZ ? wValue : L"";

    // Publisher
    wSize = sizeof(wValue);
    status = RegQueryValueExW(hKey, L"Publisher", NULL, &dwType, (LPBYTE)wValue, &wSize);
    p->Publisher = status == ERROR_SUCCESS && dwType == REG_SZ ? wValue : L"";

    // EstimatedSize
    dwSize = sizeof(dwValue);
    status = RegQueryValueExW(hKey, L"EstimatedSize", NULL, &dwType, (LPBYTE)&dwValue, &dwSize);
    p->EstimatedSize = status == ERROR_SUCCESS && dwType == REG_DWORD ? SimplifyMetric(dwValue, U_KILOBYTE) : L"0 MB";

    // CurrentUserOnly
    p->CurrentUserOnly = currentUser;

    RegCloseKey(hKey);
    return true;
}

static bool query_wmi(LPCWSTR classname, const std::vector<LPCWSTR>& fields, std::vector<std::vector<VARIANT>>& variants) {
    if (!pSvc) {
        _error = L"IWbemServices was not initialized";
        return false;
    }

    std::wstringstream ss;
    HRESULT hr;
    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR language = SysAllocString(L"WQL");

    ss << L"SELECT * FROM " << classname;
    BSTR query = SysAllocString(ss.str().c_str());

    hr = pSvc->ExecQuery(
        language,
        query,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    SysFreeString(language);
    SysFreeString(query);

    if (FAILED(hr))
    {
        ss << "Query failed. "
            << "Error code = 0x"
            << std::hex << hr;

        _error = ss.str();
        return false;
    }

    IWbemClassObject* pclsObj;
    ULONG uReturn = 0;
    size_t len_fields = fields.size();

    while (pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
            break;

        std::vector<VARIANT>& variants_row = variants.emplace_back();

        for (int i = 0; i < len_fields; i++) {
            hr = pclsObj->Get(fields[i], 0, &variants_row.emplace_back(), 0, 0);
        }

        pclsObj->Release();
    }

    pEnumerator->Release();
    return true;
}

static void clear_variants(std::vector<VARIANT>& variants)
{
    for (VARIANT& vt : variants)
        VariantClear(&vt);
}

static void clear_variants(std::vector<std::vector<VARIANT>>& variants, bool clear_vector = false)
{
    for (std::vector<VARIANT>& row : variants)
        clear_variants(row);

    if (clear_vector)
        variants.clear();
}

bool sysinfo::Init()
{
    HRESULT hr;
    std::wstringstream ss;

    // Initialize COM
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        ss << "Failed to initialize COM library. Error code = 0x" << std::hex << hr;
        _error = ss.str();
        return false;
    }

    // Set general COM security levels
    hr = CoInitializeSecurity(
        NULL,                        // Security descriptor    
        -1,                          // COM negotiates authentication service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication level for proxies
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities of the client or server
        NULL);                       // Reserved

    if (FAILED(hr) && hr != RPC_E_TOO_LATE)
    {
        ss << "Failed to initialize security. Error code = 0x" << std::hex << hr;
        _error = ss.str();

        CoUninitialize();
        return false;
    }

    // Initialize IWbemLocator
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hr))
    {
        ss << "Failed to create IWbemLocator object. Err code = 0x" << std::hex << hr;
        _error = ss.str();

        CoUninitialize();
        return false;
    }

    // Connect to the root\default namespace with the current user.
    BSTR _namespace = SysAllocString(L"ROOT\\CIMV2");
    hr = pLoc->ConnectServer(
        _namespace,  //namespace
        NULL,        // User name 
        NULL,        // User password
        0,           // Locale 
        0,           // Security flags
        0,           // Authority 
        0,           // Context object 
        &pSvc);      // IWbemServices proxy

    SysFreeString(_namespace);

    if (FAILED(hr))
    {
        ss << "Could not connect. Error code = 0x" << std::hex << hr;
        _error = ss.str();
		sysinfo::Cleanup();
        return false;
    }

    // Set the proxy so that impersonation of the client occurs.
    hr = CoSetProxyBlanket(pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE
    );

    if (FAILED(hr))
    {
        ss << "Could not set proxy blanket. Error code = 0x" << std::hex << hr;
        _error = ss.str();
        sysinfo::Cleanup();
        return false;
    }

    return coInitialized = true;
}

void sysinfo::Cleanup()
{
    if (!coInitialized)
        return;
    
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    pSvc = NULL;
    pLoc = NULL;
    coInitialized = false;
}

std::wstring sysinfo::GetLastError() { return _error; }

bool sysinfo::GetDisks(std::vector<disk>* out)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_DiskDrive", FIELDS_DISK_DRIVE, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        std::wstring size_str = row[2].vt == VT_BSTR ? SimplifyMetric(row[2].bstrVal, unit::U_BYTE) : L"0 MB";

        out->emplace_back(
            (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),    // name
            (row[1].vt == VT_BSTR ? row[1].bstrVal : L""),    // seriaNumber
            size_str,                                         // size
            (row[3].vt == VT_BSTR ? row[3].bstrVal : L"")     // model
        );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::GetNetworkAdapters(std::vector<network_adapter>* out, int flags)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_NetworkAdapter", FIELDS_NETWORK_ADPATER, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        if (!(flags & IGNORE_EMPTY_MAC) || row[1].vt == VT_BSTR)
            out->emplace_back(
                (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),      // name
                (row[1].vt == VT_BSTR ? row[1].bstrVal : L"")       // mac
            );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::GetPhysicalMemories(std::vector<physical_memory>* out)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_PhysicalMemory", FIELDS_PHYSICAL_MEMORY, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        std::wstring capacity_str = row[1].vt == VT_BSTR ? SimplifyMetric(row[1].bstrVal, unit::U_BYTE) : L"0 MB";
        std::wstring speed_str = row[2].vt == VT_I4 ? std::to_wstring(row[2].lVal) : L"0";
        speed_str += L" MT/s";

        out->emplace_back(
            (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),    // name
            capacity_str,                                     // capacity
            speed_str                                         // speed
            );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::GetPrograms(std::vector<program>* programs, int flags)
{
    HKEY hKey = NULL;
    const size_t numRoots = 2;
    const HKEY roots[numRoots] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };

    for (int i = 0; i < numRoots; i++) {
        HKEY root = roots[i];
        bool currentUser = root == HKEY_CURRENT_USER;
        bool ignore =
            (root == HKEY_LOCAL_MACHINE && !(flags & LOCAL_MACHINE)) ||
            (currentUser && !(flags & CURRENT_USER));

        if (ignore)
            continue;

        if (RegOpenKeyExW(root, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return false;

        WCHAR    achKey[255];               // Buffer para o nome da subchave
        DWORD    cbName;                    // Tamanho do nome
        FILETIME ftLastWriteTime;           // Tempo da última modificação
        DWORD    retCode;

        DWORD cSubKeys = 0;
        RegQueryInfoKeyW(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        if (cSubKeys > 0) {
            bool construct_new = true; // aproveita o objeto program caso get_program não o utilize (em caso de erro ou IGNORE_SUBSOFTWARE)
            program* p = NULL;

            for (DWORD j = 0; j < cSubKeys; j++) {
                cbName = 255;
                retCode = RegEnumKeyExW(hKey, j, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);

                if (retCode == ERROR_SUCCESS) {

                    if (construct_new) {
                        p = &programs->emplace_back();
                    }

                    construct_new = get_program(hKey, achKey, p, flags, currentUser); // caso p seja utilizado
                }
            }

            if (!construct_new) // caso o ultimo objeto gerado não tenha sido utilizado
                programs->pop_back();
        }
    }

    RegCloseKey(hKey);
    return true;
}

bool sysinfo::GetMotherboard(std::wstring* name, std::wstring* manufacturer)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_BaseBoard", FIELDS_BASE_BOARD, variants) || !variants.size())
        return false;

    auto& row = variants[0];

    *manufacturer = row[0].vt == VT_BSTR ? row[0].bstrVal : L"";
    *name = row[1].vt == VT_BSTR ? row[1].bstrVal : L"";

    clear_variants(variants);
    return true;
}

bool sysinfo::GetProcessor(std::wstring* name, std::wstring* clock_speed)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_Processor", FIELDS_PROCESSOR, variants) || !variants.size())
        return false;

    auto& row = variants[0];
    wchar_t buffer[50];
    swprintf(buffer, 50, L"%.2f GHz", (row[1].vt == VT_I4 ? (row[1].lVal / 1000.0) : 0));

    *name = row[0].vt == VT_BSTR ? row[0].bstrVal : L"";
    *clock_speed = buffer;

    StripWString(name);

    clear_variants(variants);
    return true;
}

bool sysinfo::GetGroups(std::vector<user_group>* out)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_Group", FIELDS_GROUP, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        out->emplace_back(
            (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),    // sid
            (row[1].vt == VT_BSTR ? row[1].bstrVal : L""),    // name
            (row[2].vt == VT_BSTR ? row[2].bstrVal : L""),    // description
            (row[3].vt == VT_BSTR ? row[3].bstrVal : L""),    // domain
            (row[4].vt == VT_BSTR ? row[4].bstrVal : L""),    // status
            (row[5].vt == VT_BOOL ? row[5].boolVal : 0)       // local
        );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::GetUserAccounts(std::vector<user_account>* out)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_UserAccount", FIELDS_USER_ACCOUNT, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        out->emplace_back(
            (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),    // sid
            (row[1].vt == VT_BSTR ? row[1].bstrVal : L""),    // name
            (row[2].vt == VT_BSTR ? row[2].bstrVal : L""),    // fullName
            (row[3].vt == VT_BSTR ? row[3].bstrVal : L""),    // description
            (row[4].vt == VT_BSTR ? row[4].bstrVal : L""),    // domain
            (row[5].vt == VT_BSTR ? row[5].bstrVal : L""),    // status
            (row[6].vt == VT_BOOL ? row[6].boolVal : 0),      // disabled
            (row[7].vt == VT_BOOL ? row[7].boolVal : 0),      // local
            (row[8].vt == VT_BOOL ? row[8].boolVal : 0),      // lockout
            (row[9].vt == VT_BOOL ? row[9].boolVal : 0),      // passwordChangeable
            (row[10].vt == VT_BOOL ? row[10].boolVal : 0),    // passwordExpires
            (row[11].vt == VT_BOOL ? row[11].boolVal : 0)     // passwordRequired
        );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::GetUserAccountGroups(user_accounts_by_group* map_group_key, user_accounts_by_group* map_account_key)
{
	if (!map_group_key && !map_account_key) {
        _error = L"At least one of the parameters must be provided";
        return false;
    }

    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_GroupUser", FIELDS_GROUP_USER, variants))
        return false;

    //std::wcout << "GetUserAccountGroups Getting values...\n";

    HRESULT hr;
    for (std::vector<VARIANT>& row : variants) {
		if (row[0].vt != VT_BSTR || row[1].vt != VT_BSTR) {
            clear_variants(row);
            continue;
        }

        IWbemClassObject* pObj = NULL;

        // coletando GroupComponent
        hr = pSvc->GetObject(row[0].bstrVal, 0, NULL, &pObj, NULL);
		if (!SUCCEEDED(hr)) {
            //std::wcout << "Error: coletando GroupComponent\n";
            clear_variants(row);
            continue;
        }

		VARIANT vtGroupSid;
		hr = pObj->Get(L"SID", 0, &vtGroupSid, NULL, NULL);
        pObj->Release();
        pObj = NULL;

        if (!SUCCEEDED(hr)) {
            clear_variants(row);
            VariantClear(&vtGroupSid);
            //std::wcout << "Error: GET SID GroupComponent\n";
            continue;
        }

        // coletando PartComponent
        hr = pSvc->GetObject(row[1].bstrVal, 0, NULL, &pObj, NULL);
        if (!SUCCEEDED(hr)) {
            clear_variants(row);
            //std::wcout << "Error: coletando PartComponent\n";
            continue;
        }

        VARIANT vtUserSid;
        hr = pObj->Get(L"SID", 0, &vtUserSid, NULL, NULL);
        pObj->Release();

        if (!SUCCEEDED(hr)) {
            clear_variants(row);
            VariantClear(&vtGroupSid);
            VariantClear(&vtUserSid);
            //std::wcout << "Error: GET SID PartComponent\n";
            continue;
        }

        std::wstring groupSid = vtGroupSid.bstrVal;
        std::wstring userSid = vtUserSid.bstrVal;
        VariantClear(&vtGroupSid);
        VariantClear(&vtUserSid);

		// User Accounts by Group
        if (map_group_key) {
            if (map_group_key->count(groupSid)) {
                map_group_key->at(groupSid).emplace_back(userSid);
            }
            else
                map_group_key->emplace(groupSid, std::vector<std::wstring>()).first->second.emplace_back(userSid);
        }

		// Groups By User Account
        if (map_account_key) {
            if (map_group_key->count(userSid)) {
                map_group_key->at(userSid).emplace_back(groupSid);
            }
            else
                map_group_key->emplace(userSid, std::vector<std::wstring>()).first->second.emplace_back(groupSid);
        }

        clear_variants(row);
    }

    return true;
}

bool sysinfo::GetMachine(machine* out, int flags)
{
    // OS
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_OperatingSystem", FIELDS_OPERATING_SYSTEM, variants))
        return false;

    // essa consulta deve retornar apenas um valor
    std::vector<VARIANT>& row = variants[0];
    out->osName = row[0].vt == VT_BSTR ? row[0].bstrVal : L"";
    out->osVersion = row[1].vt == VT_BSTR ? row[1].bstrVal : L"";
    out->osInstallDate = row[2].vt == VT_BSTR ? row[2].bstrVal : L"";
    out->osArchitecture = row[3].vt == VT_BSTR ? row[3].bstrVal : L"";
    out->osSerialNumber = row[4].vt == VT_BSTR ? row[4].bstrVal : L"";
    out->osOrganization = row[5].vt == VT_BSTR ? row[5].bstrVal : L"";

    clear_variants(variants);

    // tratando osInstallDate, padrão 20260119151523.000000-180
    if (out->osInstallDate.size() >= 14) {
        std::wstring& d = out->osInstallDate;
        out->osInstallDate =
            d.substr(0, 4)
            + L"-" + d.substr(4, 2)
            + L"-" + d.substr(6, 2)
            + L" " + d.substr(8, 2)
            + L":" + d.substr(10, 2)
            + L":" + d.substr(12, 2);
    }

    // Informações Adicionais
    return
        GetMotherboard(&out->motherboardName, &out->motherboardManufacturer)
        && GetProcessor(&out->processorName, &out->processorClockSpeed)
        && GetDisks(&out->disks)
        && GetNetworkAdapters(&out->network_adapters, flags)
        && GetPhysicalMemories(&out->physical_memories)
        && GetPrograms(&out->programs, flags)
        && GetGroups(&out->groups)
        && GetUserAccounts(&out->accounts)
        && GetUserAccountGroups(&out->group_members, nullptr);
}

cJSON* sysinfo::MachineToJson(const machine* data)
{
    cJSON
        *json = NULL,
        *jobject = NULL,
        *jarray_disks = NULL,
        *jarray_network_adapters = NULL,
        *jarray_physical_memories = NULL,
        *jarray_programs = NULL,
        *jarray_accounts = NULL,
        *jarray_groups = NULL,
        *jarray_group_members = NULL;

    // gerando JSON do objeto inicial
    if (!(json = cJSON_CreateObject())) {
        _error = L"it was not possible to create the JSON object";
        return NULL;
    }

    if (
        !cJSON_AddStringToObject(json, "os", ToString(data->osName).c_str()) ||
        !cJSON_AddStringToObject(json, "osArchitecture", ToString(data->osArchitecture).c_str()) ||
        !cJSON_AddStringToObject(json, "osInstallDate", ToString(data->osInstallDate).c_str()) ||
        !cJSON_AddStringToObject(json, "osVersion", ToString(data->osVersion).c_str()) ||
        !cJSON_AddStringToObject(json, "osSerialNumber", ToString(data->osSerialNumber).c_str()) ||
        !cJSON_AddStringToObject(json, "osOrganization", ToString(data->osOrganization).c_str()) ||
        !cJSON_AddStringToObject(json, "motherboard", ToString(data->motherboardName).c_str()) ||
        !cJSON_AddStringToObject(json, "motherboardManufacturer", ToString(data->motherboardManufacturer).c_str()) ||
        !cJSON_AddStringToObject(json, "processor", ToString(data->processorName).c_str()) ||
        !cJSON_AddStringToObject(json, "processorClockSpeed", ToString(data->processorClockSpeed).c_str()) ||
        !(jarray_disks = cJSON_AddArrayToObject(json, "disks")) ||
        !(jarray_network_adapters = cJSON_AddArrayToObject(json, "networkAdapters")) ||
        !(jarray_physical_memories = cJSON_AddArrayToObject(json, "physicalMemories")) ||
        !(jarray_programs = cJSON_AddArrayToObject(json, "programs")) ||
        !(jarray_accounts = cJSON_AddArrayToObject(json, "accounts")) ||
        !(jarray_groups = cJSON_AddArrayToObject(json, "groups")) ||
        !(jarray_group_members = cJSON_AddArrayToObject(json, "groupMembers"))
        ) {
        _error = L"it was not possible to add all JSON properties";
        cJSON_Delete(json);
        return NULL;
    }

    // adicionando discos
    for (const disk& d : data->disks) {
        if (
            !(jobject = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_disks, jobject) ||
            !cJSON_AddStringToObject(jobject, "name", ToString(d.name).c_str()) ||
            !cJSON_AddStringToObject(jobject, "seriaNumber", ToString(d.seriaNumber).c_str()) ||
            !cJSON_AddStringToObject(jobject, "size", ToString(d.size).c_str()) ||
            !cJSON_AddStringToObject(jobject, "model", ToString(d.model).c_str())
            ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

    // adicionando adaptadores
    for (const network_adapter& adapter : data->network_adapters) {
        if (
            !(jobject = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_network_adapters, jobject) ||
            !cJSON_AddStringToObject(jobject, "name", ToString(adapter.name).c_str()) ||
            !cJSON_AddStringToObject(jobject, "mac", ToString(adapter.mac).c_str())
            ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

    // adicionando memórias
    for (const physical_memory& memory : data->physical_memories) {
        if (
            !(jobject = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_physical_memories, jobject) ||
            !cJSON_AddStringToObject(jobject, "name", ToString(memory.name).c_str()) ||
            !cJSON_AddStringToObject(jobject, "capacity", ToString(memory.capacity).c_str()) ||
            !cJSON_AddStringToObject(jobject, "speed", ToString(memory.speed).c_str())
            ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

    // adicionando programas
    for (const program& p : data->programs) {
        if (
            !(jobject = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_programs, jobject) ||
            !cJSON_AddStringToObject(jobject, "name", ToString(p.DisplayName).c_str()) ||
            !cJSON_AddStringToObject(jobject, "version", ToString(p.DisplayVersion).c_str()) ||
            !cJSON_AddStringToObject(jobject, "publisher", ToString(p.Publisher).c_str()) ||
            !cJSON_AddStringToObject(jobject, "estimatedSize", ToString(p.EstimatedSize).c_str()) ||
            !cJSON_AddBoolToObject(jobject, "currentUserOnly", p.CurrentUserOnly)
            ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

	// adicionando contas de usuário
	for (const user_account& account : data->accounts) {
        if (
            !(jobject = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_accounts, jobject) ||
            !cJSON_AddStringToObject(jobject, "sid", ToString(account.sid).c_str()) ||
            !cJSON_AddStringToObject(jobject, "name", ToString(account.name).c_str()) ||
            !cJSON_AddStringToObject(jobject, "fullName", ToString(account.fullName).c_str()) ||
            !cJSON_AddStringToObject(jobject, "description", ToString(account.description).c_str()) ||
            !cJSON_AddStringToObject(jobject, "domain", ToString(account.domain).c_str()) ||
            !cJSON_AddStringToObject(jobject, "status", ToString(account.status).c_str()) ||
            !cJSON_AddBoolToObject(jobject, "disabled", account.disabled) ||
            !cJSON_AddBoolToObject(jobject, "local", account.local) ||
            !cJSON_AddBoolToObject(jobject, "lockout", account.lockout) ||
            !cJSON_AddBoolToObject(jobject, "passwordChangeable", account.passwordChangeable) ||
            !cJSON_AddBoolToObject(jobject, "passwordExpires", account.passwordExpires) ||
            !cJSON_AddBoolToObject(jobject, "passwordRequired", account.passwordRequired)
            ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

    // adicionando grupos
	for (const user_group& group : data->groups) {
        if (
            !(jobject = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_groups, jobject) ||
            !cJSON_AddStringToObject(jobject, "sid", ToString(group.sid).c_str()) ||
            !cJSON_AddStringToObject(jobject, "name", ToString(group.name).c_str()) ||
            !cJSON_AddStringToObject(jobject, "description", ToString(group.description).c_str()) ||
            !cJSON_AddStringToObject(jobject, "domain", ToString(group.domain).c_str()) ||
            !cJSON_AddStringToObject(jobject, "status", ToString(group.status).c_str()) ||
            !cJSON_AddBoolToObject(jobject, "local", group.local)
            ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

	// adicionando membros de grupos
	for (const auto& pair : data->group_members) {
        const std::wstring& groupSid = pair.first;
        const std::vector<std::wstring>& userSids = pair.second;
        for (const std::wstring& userSid : userSids) {
            if (
                !(jobject = cJSON_CreateObject()) ||
                !cJSON_AddItemToArray(jarray_group_members, jobject) ||
                !cJSON_AddStringToObject(jobject, "groupSid", ToString(groupSid).c_str()) ||
                !cJSON_AddStringToObject(jobject, "accountSid", ToString(userSid).c_str())
                ) {
                _error = L"it wasn't possible to add all JSON properties";
                cJSON_Delete(json);
                return NULL;
            }
        }
    }

    return json;
}

bool sysinfo::MachineFromJString(const char* str_data, machine* out)
{
    return false;
}
