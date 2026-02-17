#include "pch.h"
#include "framework.h"

#include "sysinfo.h"
#include "utils.h"
#include <sstream>

static IWbemServices* pSvc = NULL;
static IWbemLocator* pLoc = NULL;
static std::wstring _error;

static const std::vector<LPCWSTR> FIELDS_LOGICAL_DISK { L"FreeSpace", L"Size" };
static const std::vector<LPCWSTR> FIELDS_DISK_DRIVE { L"Caption", L"SerialNumber", L"Size", L"Model" };
static const std::vector<LPCWSTR> FIELDS_NETWORK_ADPATER { L"Name", L"MACAddress" };
static const std::vector<LPCWSTR> FIELDS_PHYSICAL_MEMORY { L"Tag", L"Capacity", L"Speed" };
static const std::vector<LPCWSTR> FIELDS_PROCESSOR { L"Name", L"CurrentClockSpeed"};
static const std::vector<LPCWSTR> FIELDS_OPERATING_SYSTEM { L"Caption", L"Version", L"InstallDate", L"OSArchitecture", L"SerialNumber", L"Organization" };
static const std::vector<LPCWSTR> FIELDS_BASE_BOARD { L"Manufacturer", L"Product" };


static bool get_program(HKEY root, LPCWSTR subkey, sysinfo::program* p, int flags, bool currentUser)
{
    HKEY hKey;
    LSTATUS status;

    if (RegOpenKeyExW(root, subkey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return false;

    // variaveis para consulta
    DWORD dwType;
    WCHAR wValue[255];
    DWORD wSize = sizeof(wValue);
    DWORD dwValue;
    DWORD dwSize = sizeof(dwValue);

    // caso seja um componente de software e IGNORE_SUBSOFTWARE esteja definido
    if ((flags & sysinfo::IGNORE_SUBSOFTWARE) && (RegQueryValueExW(hKey, L"ParentKeyName", NULL, NULL, NULL, NULL) == ERROR_SUCCESS)) {
        RegCloseKey(hKey);
        return false;
    }

    // DisplayName
    status = RegQueryValueExW(hKey, L"DisplayName", NULL, &dwType, (LPBYTE)wValue, &wSize);

    // caso DisplayName venha em branco e IGNORE_EMPTY esteja definido
    if (flags & sysinfo::IGNORE_EMPTY_PROGRAM && (dwType == VT_EMPTY || status != ERROR_SUCCESS)) {
        RegCloseKey(hKey);
        return false;
    }

    p->DisplayName = status == ERROR_SUCCESS && dwType == REG_SZ ? wValue : L"";


    // DisplayVersion
    status = RegQueryValueExW(hKey, L"DisplayVersion", NULL, &dwType, (LPBYTE)wValue, &wSize);
    p->DisplayVersion = status == ERROR_SUCCESS && dwType == REG_SZ ? wValue : L"";

    // Publisher
    status = RegQueryValueExW(hKey, L"Publisher", NULL, &dwType, (LPBYTE)wValue, &wSize);
    p->Publisher = status == ERROR_SUCCESS && dwType == REG_SZ ? wValue : L"";

    // EstimatedSize
    status = RegQueryValueExW(hKey, L"EstimatedSize", NULL, &dwType, (LPBYTE)&dwValue, &dwSize);
    p->EstimatedSize = status == ERROR_SUCCESS && dwType == REG_DWORD ? dwValue : 0;

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

bool sysinfo::init()
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

    if (FAILED(hr))
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

        pLoc->Release();
        pLoc = NULL;
        CoUninitialize();
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

        cleanup();
        return false;
    }

    return true;
}

void sysinfo::cleanup()
{
    if (pSvc) {
        pSvc->Release();
        pSvc = NULL;
    }

    if (pLoc) {
        pLoc->Release();
        pLoc = NULL;
    }

    CoUninitialize();
}

std::wstring sysinfo::get_last_error() { return _error; }

bool sysinfo::get_operating_system_name(std::wstring* out)
{
    HRESULT hr;
    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR language = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM Win32_OperatingSystem");
    std::wstringstream ss;

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
        ss << "Query for processes failed. " << "Error code = 0x" << std::hex << hr;
        _error = ss.str();
        return false;
    }

    IWbemClassObject* pclsObj;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
            break;

        VARIANT vtProp;
        hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);

        if (FAILED(hr)) {
            ss << "Get property Caption failed. " << "Error code = 0x" << std::hex << hr;
            pEnumerator->Release();
            _error = ss.str();
            return false;
        }

        *out = vtProp.bstrVal;
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    pEnumerator->Release();
    return true;
}

bool sysinfo::get_disks(std::vector<disk>* out)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_DiskDrive", FIELDS_DISK_DRIVE, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        out->emplace_back(
            (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),    // name
            (row[1].vt == VT_BSTR ? row[1].bstrVal : L""),    // seriaNumber
            (row[2].vt == VT_BSTR ? row[2].bstrVal : L""),    // size
            (row[3].vt == VT_BSTR ? row[3].bstrVal : L"")     // model
        );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::get_network_adapters(std::vector<network_adapter>* out, int flags)
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

bool sysinfo::get_physical_memories(std::vector<physical_memory>* out)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_PhysicalMemory", FIELDS_PHYSICAL_MEMORY, variants))
        return false;

    for (std::vector<VARIANT>& row : variants) {
        out->emplace_back(
            (row[0].vt == VT_BSTR ? row[0].bstrVal : L""),    // name
            (row[1].vt == VT_BSTR ? row[1].bstrVal : L""),    // capacity
            (row[2].vt == VT_I4 ? row[2].lVal : 0),           // speed
            L"B",                                            // capaticyUnit
            L"MT/s"                                          // speedUnit
        );

        clear_variants(row);
    }

    return true;
}

bool sysinfo::get_programs(std::vector<program>* programs, int flags)
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

bool sysinfo::get_motherboard(std::wstring* name, std::wstring* manufacturer)
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

bool sysinfo::get_processor(std::wstring* name, long* clock_speed)
{
    std::vector<std::vector<VARIANT>> variants;
    if (!query_wmi(L"Win32_Processor", FIELDS_PROCESSOR, variants) || !variants.size())
        return false;

	auto& row = variants[0];

    *name = row[0].vt == VT_BSTR ? row[0].bstrVal : L"";
    *clock_speed = row[1].vt == VT_I4 ? row[1].lVal : 0;

    clear_variants(variants);
    return true;
}

bool sysinfo::get_machine(machine* out, int flags)
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
        get_motherboard(&out->motherboardName, &out->motherboardManufacturer)
        && get_processor(&out->processorName, &out->processorClockSpeed)
        && get_disks(&out->disks)
        && get_network_adapters(&out->network_adapters, flags)
        && get_physical_memories(&out->physical_memories)
        && get_programs(&out->programs, flags);
}

cJSON* sysinfo::machine_to_cjson(const machine* data)
{
    cJSON
        *json = NULL,
        *jarray_network_adapters = NULL,
        *jobject_network_adapter = NULL;

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
        !cJSON_AddNumberToObject(json, "processorClockSpeed", data->processorClockSpeed) ||
        !(jarray_network_adapters = cJSON_AddArrayToObject(json, "networkAdapters"))
    ) {
        _error = L"it was not possible to add all JSON properties";
        cJSON_Delete(json);
        return NULL;
    }

    // adicionando adaptadores
    for (const network_adapter& adapter : data->network_adapters) {
        if (
            !(jobject_network_adapter = cJSON_CreateObject()) ||
            !cJSON_AddItemToArray(jarray_network_adapters, jobject_network_adapter) ||
            !cJSON_AddStringToObject(jobject_network_adapter, "name", ToString(adapter.name).c_str()) ||
            !cJSON_AddStringToObject(jobject_network_adapter, "mac", ToString(adapter.mac).c_str())
        ) {
            _error = L"it wasn't possible to add all JSON properties";
            cJSON_Delete(json);
            return NULL;
        }
    }

    return json;
}