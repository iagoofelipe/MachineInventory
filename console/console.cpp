#include "console.h"
#include "utils.h"
#include "cJSON.h"

#include <iostream>
#include <sstream>
#include <curl/curl.h>

static CURL* curl = NULL;

static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static bool get_request(const std::string& url, response* r)
{
	r->status_code = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r->body);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
		std::wcout << "[get_request ERROR] " << curl_easy_strerror(res) << std::endl;
    }
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r->status_code);
    }

    curl_easy_reset(curl);

	return res == CURLE_OK;
}

static bool post_request(const std::string& url, const std::string& data, response* r)
{
    r->status_code = 0;
    struct curl_slist* headers = NULL;
    CURLcode res;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r->body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::wcout << "[post_request ERROR] " << curl_easy_strerror(res) << std::endl;
    }
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r->status_code);
    }

    curl_easy_reset(curl);

    return res == CURLE_OK;
}

bool init_console()
{
    curl = curl_easy_init();
    if(!curl) {
        std::wcout << "[init_console ERROR] Failed to initialize CURL" << std::endl;
        return false;
	}

    configure_terminal();

    return true;
}

void cleanup_console()
{
    if (curl) {
        curl_easy_cleanup(curl);
        curl = NULL;
    }
}

void show_machine(const sysinfo::machine& machine)
{
    int i;

    std::wcout
        << "Operating System: " << machine.osName << std::endl
        << "Architecture: " << machine.osArchitecture << std::endl
        << "Install Date: " << machine.osInstallDate << std::endl
        << "Version: " << machine.osVersion << std::endl
        << "Organization: " << machine.osOrganization << std::endl
        << "Serial Number: " << machine.osSerialNumber << std::endl
        << "Motherboard Name: " << machine.motherboardName << std::endl
        << "Motherboard Manufacturer: " << machine.motherboardManufacturer << std::endl
        << "Processor Name: " << machine.processorName << std::endl
        << "Processor Clock Speed: " << machine.processorClockSpeed << std::endl;

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
}

bool upload_machine(const sysinfo::machine& machine, const wchar_t* cpf, const wchar_t* title)
{
    std::wstring _cpf;

    if (!cpf) {
        std::wcout << "User's CPF (000.000.000-00): ";
        std::getline(std::wcin, _cpf);
    }
    else
		_cpf = cpf;

	// query the user's ID by CPF
    std::wcout << "[upload_machine] getting the user's ID..." << std::endl;
    response r;
    if (!get_request("http://127.0.0.1:5000/user/userByCPF/" + ToString(_cpf), &r))
        return false;

    if (r.status_code == 404) {
        std::wcout << "[upload_machine ERROR] User not found with CPF '" << cpf << "'" << std::endl;
        return false;
	}

    else if (r.status_code != 200) {
        std::wcout << "[upload_machine ERROR] User not found with CPF, ResponseCode: " << r.status_code << std::endl;
        return false;
	}

	cJSON* json = cJSON_Parse(r.body.c_str());
    if (!json) {
        std::wcout << "[upload_machine ERROR] Failed to parse JSON response" << std::endl;
        return false;
    }

    cJSON* id_item = cJSON_GetObjectItem(json, "id");
    if (!id_item || id_item->type != cJSON_String) {
        std::wcout << "[upload_machine ERROR] Invalid JSON response: 'id' field is missing or not a string" << std::endl;
        cJSON_Delete(json);
        return false;
    }

    std::string user_id = id_item->valuestring;
    std::wcout << "[upload_machine] User ID: " << id_item->valuestring << std::endl;

	// sending machine data to the server
    std::wstringstream wss;
    std::wstring _title;

    std::wcout << "[upload_machine] uploading machine data..." << std::endl;

    if (!title) {
        std::wcout << "Enter a title for this machine: ";
        std::getline(std::wcin, _title);
    }
    else
		_title = title;

    wss
        << "{\n"
        << "  \"owner_id\": \"" << user_id.c_str() << "\",\n"
        << "  \"title\": \"" << _title << "\",\n"
        << "  \"os\": \"" << machine.osName << "\",\n"
        << "  \"os_architecture\": \"" << machine.osArchitecture << "\",\n"
        << "  \"os_install_date\": \"" << machine.osInstallDate << "\",\n"
        << "  \"os_version\": \"" << machine.osVersion << "\",\n"
        << "  \"os_serial_number\": \"" << machine.osSerialNumber << "\",\n"
        << "  \"organization\": \"" << machine.osOrganization << "\",\n"
        << "  \"motherboard\": \"" << machine.motherboardName << "\",\n"
        << "  \"motherboard_manufacturer\": \"" << machine.motherboardManufacturer << "\",\n"
        << "  \"processor\": \"" << machine.processorName << "\",\n"
		<< "  \"processor_clock_speed\": " << machine.processorClockSpeed << ",\n"
        << "  \"network_adapters\": [\n";

    int i = 0;
    size_t index_last = machine.network_adapters.size() - 1;

	for (const sysinfo::network_adapter& adapter : machine.network_adapters) {
        wss
        << "    {\n"
        << "      \"name\": \"" << adapter.name << "\",\n"
        << "      \"mac\": \"" << adapter.mac << "\"\n"
        << "    }" << (i++ != index_last ? "," : "") << "\n";
    }

	wss << "  ]\n}";

    std::wstring wdata = wss.str();
    std::string data = ToString(wdata);
    std::wcout << "[upload_machine] JSON data to upload:\n" << wdata << std::endl;

    if (!post_request("http://127.0.0.1:5000/machine/newMachine", data, &r)) {
        return false;
    }

    if (r.status_code != 200) {
        std::wcout << "[upload_machine ERROR] Failed to upload machine data, ResponseCode: " << r.status_code << std::endl;
        return false;
    }

	cJSON_Delete(json);
    return true;
}