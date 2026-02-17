#include "pch.h"
#include "framework.h"

#include "server.h"
#include "cJSON.h"
#include "utils.h"
#include <iostream>
#include <fstream>

const std::string sysinfo::ServerConnection::BASE_URL = "http://127.0.0.1:5000";

static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

sysinfo::ServerConnection::ServerConnection()
{
    curl = curl_easy_init();
    if(!curl) {
        last_error = "Failed to initialize CURL";
        return;
	}

    std::ifstream token_file("token.txt");
    if (!token_file.is_open()) {
        last_error = "Token not found in memory 'token.txt'";
        return;
    }

    std::getline(token_file, token);
    token_file.close();
}

sysinfo::ServerConnection::~ServerConnection()
{
    curl_easy_cleanup(curl);
}

bool sysinfo::ServerConnection::validate_token()
{
    if (token.empty()) {
        last_error = "Token not found in memory 'token.txt'";
		return false;
	}

    response r;
    if (get_request(BASE_URL + "/auth/validateToken", &r) && r.status_code == 200)
        return true;

    last_error = "Invalid token, ResponseCode: " + std::to_string(r.status_code) + ", Content: " + r.content;
    return false;
}

bool sysinfo::ServerConnection::login(const std::string& cpf, const std::string& password)
{
    response r;
	post_request(BASE_URL + "/auth", "{\"cpf\":\"" + cpf + "\",\"password\":\"" + password + "\"}", &r);
    if (r.status_code != 200) {
        last_error = "Login failed, ResponseCode: " + std::to_string(r.status_code) + ", Content: " + r.content;
        return false;
	}

	// coletando token
    cJSON* json, *json_token;

    if (!(json = cJSON_Parse(r.content.c_str()))) {
        last_error = "it wasn't possible to parse the JSON from the server response";
        return false;
    }

    if (
        !(json_token = cJSON_GetObjectItem(json, "token")) ||
        !cJSON_IsString(json_token)
        ) {
		last_error = "it wasn't possible to get the token from the server response";
        cJSON_Delete(json);
        return false;
    }

	token = json_token->valuestring;
    std::ofstream token_file("token.txt");
    if (token_file.is_open()) {
        token_file << token;
        token_file.close();
    }
    else {
        last_error = "Failed to save token to 'token.txt'";
        cJSON_Delete(json);
		return false;
    }

	cJSON_Delete(json);
    return true;
}

// bool sysinfo::ServerConnection::get_user_id_by_cpf(const std::string& cpf, std::string *id)
// {
//     response r;
//     if (!get_request(BASE_URL + "/user/" + cpf, &r))
//         return false;

//     if (r.status_code != 200) {
//         last_error = (std::string)"User not found with CPF '" + cpf + "', ResponseCode " + std::to_string(r.status_code);
//         return false;
// 	}

// 	cJSON* json = cJSON_Parse(r.content.c_str());
//     if (!json) {
//         last_error = "Failed to parse JSON response";
//         return false;
//     }

//     cJSON* id_item = cJSON_GetObjectItem(json, "id");
//     if (!id_item || id_item->type != cJSON_String) {
//         last_error = "Invalid JSON response: 'id' field is missing or not a string";
//         cJSON_Delete(json);
//         return false;
//     }

//     *id = id_item->valuestring;
//     cJSON_Delete(json);
//     return true;
// }

bool sysinfo::ServerConnection::create_new_user(const std::string &cpf, const std::string &name, const std::string& password, std::string *id)
{
    cJSON *json = cJSON_CreateObject();

    if (!json) {
        last_error = "it was not possible to create the JSON object";
        return false;
    }
    
    if(
        !cJSON_AddStringToObject(json, "cpf", cpf.c_str()) ||
        !cJSON_AddStringToObject(json, "name", name.c_str()) ||
        !cJSON_AddStringToObject(json, "password", password.c_str())
    ) {
        cJSON_Delete(json);
        last_error = "it was not possible to create the JSON object";
        return false;
    }

    response r;
    post_request(BASE_URL + "/user", cJSON_PrintUnformatted(json), &r);
    cJSON_Delete(json);
    json = NULL;

    if (r.status_code != 200) {
        last_error = "it wasn't possible to create the new user, ResponseCode: " + std::to_string(r.status_code) + ", Content: " + r.content;
        return false;
    }

    // coletando id criado
    return get_id_from_response(r.content, id);
}

bool sysinfo::ServerConnection::upload_machine(const machine* data, const char* ownerCpf, const char* machineTitle, std::string* id)
{
    cJSON *json = machine_to_cjson(data);
    if (!json) {
        last_error = "it was not possible to create the JSON object";
        return false;
    }

    bool success = upload_machine(json, ownerCpf, machineTitle, id);
	cJSON_Delete(json);

	return success;
}

bool sysinfo::ServerConnection::upload_machine(cJSON* json, const char* ownerCpf, const char* machineTitle, std::string* id)
{
    if (
        !cJSON_AddStringToObject(json, "ownerCpf", ownerCpf) ||
        !cJSON_AddStringToObject(json, "title", machineTitle)
    ) {
        last_error = "it was not possible to create the JSON object";
        return false;
    }

    // enviando dados para o servidor
    response r;
    post_request(BASE_URL + "/machine", cJSON_PrintUnformatted(json), &r);

    if (r.status_code != 200) {
        last_error = "Failed to upload machine data, ResponseCode: " + std::to_string(r.status_code) + ", Content: " + r.content;
        return false;
    }

    // coletando id criado
    return get_id_from_response(r.content, id);
}

bool sysinfo::ServerConnection::get_request(const std::string& url, response* r)
{
    r->status_code = 0;
    curl_slist* headers = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r->content);

    headers = curl_slist_append(headers, ("Authorization: " + token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK)
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r->status_code);
    else
        last_error = curl_easy_strerror(res);

    curl_easy_reset(curl);

	return res == CURLE_OK;
}

bool sysinfo::ServerConnection::post_request(const std::string& url, const std::string& data, response* r)
{
    r->status_code = 0;
    curl_slist* headers = NULL;
    CURLcode res;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r->content);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: " + token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK)
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r->status_code);
    else
        last_error = curl_easy_strerror(res);

    curl_easy_reset(curl);

    return res == CURLE_OK;
}

bool sysinfo::ServerConnection::get_id_from_response(const std::string& content, std::string *id)
{
    cJSON
        *json = NULL,
        *json_id = NULL;

    if ((json = cJSON_Parse(content.c_str())) == NULL) {
        last_error = "it wasn't possible to extract the JSON from the server response";
        return false;
    }

    if (
        !cJSON_HasObjectItem(json, "id") ||
        !(json_id = cJSON_GetObjectItem(json, "id")) ||
        !cJSON_IsString(json_id)
    ) {
        last_error = "it wasn't possible to get the id from the server response";
        cJSON_Delete(json);
        return false;
    }

    *id = json_id->valuestring;
    cJSON_Delete(json);
    return true;
}