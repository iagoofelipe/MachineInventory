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

const char* sysinfo::ServerConnection::get_last_error() { return last_error.c_str(); }
const char* sysinfo::ServerConnection::get_token() { return token.c_str(); }
bool sysinfo::ServerConnection::is_ready() { return curl; }
bool sysinfo::ServerConnection::has_token() { return !token.empty(); }

bool sysinfo::ServerConnection::validate_token()
{
    if (token.empty()) {
        last_error = "Token not found in memory 'token.txt'";
		return false;
	}

    response r;
    if (!get_request(BASE_URL + "/auth/validateToken", &r))
        return false;

    if (r.status_code != 200) {
		get_sfield_from_jstring(r.content, "message", &last_error);
        return false;
    }

    return true;
}

bool sysinfo::ServerConnection::login(const std::string& cpf, const std::string& password)
{
    response r;
	if (!post_request(BASE_URL + "/auth", "{\"cpf\":\"" + cpf + "\",\"password\":\"" + password + "\"}", &r)) {
        return false;
	}

    if (r.status_code != 200) {
		get_sfield_from_jstring(r.content, "message", &last_error);
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

bool sysinfo::ServerConnection::get_user(user* u)
{
	return get_user("", u);
}

bool sysinfo::ServerConnection::get_user(const std::string& cpf_or_id, user* u)
{
	response r;
    std::string url = BASE_URL + "/user" + (cpf_or_id.empty() ? "" : "/" + cpf_or_id);

    if (!get_request(url, &r)) {
        last_error = "Failed to get user data, ResponseCode: " + std::to_string(r.status_code);
        return false;
    }

    if (r.status_code != 200) {
        get_sfield_from_jstring(r.content, "message", &last_error);
        return false;
    }

	cJSON *json, *json_id, *json_cpf, *json_name;
    if (!(json = cJSON_Parse(r.content.c_str()))) {
        last_error = "it wasn't possible to parse the JSON from the server response";
		return false;
    }

    if (
        !(json_id = cJSON_GetObjectItem(json, "id")) || !cJSON_IsString(json_id) ||
        !(json_cpf = cJSON_GetObjectItem(json, "cpf")) || !cJSON_IsString(json_cpf) ||
        !(json_name = cJSON_GetObjectItem(json, "name")) || !cJSON_IsString(json_name)
    ) {
        last_error = "it wasn't possible to parse the user data from the server response";
        cJSON_Delete(json);
        return false;
	}

    u->id = json_id->valuestring;
    u->cpf = json_cpf->valuestring;
    u->name = json_name->valuestring;

    cJSON_Delete(json);
    return true;
}

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
    bool request_success = post_request(BASE_URL + "/user", cJSON_PrintUnformatted(json), &r);
    cJSON_Delete(json);
    json = NULL;

	if (!request_success) {
        return false;
    }

    if (r.status_code != 200) {
		get_sfield_from_jstring(r.content, "message", &last_error);
        return false;
    }

    // coletando id criado
    return get_sfield_from_jstring(r.content, "id", id);
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
    if (!post_request(BASE_URL + "/machine", cJSON_PrintUnformatted(json), &r))
        return false;

    if (r.status_code != 200) {
		get_sfield_from_jstring(r.content, "message", &last_error);
        return false;
    }

    // coletando id criado
    return get_sfield_from_jstring(r.content, "id", id);
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

bool sysinfo::ServerConnection::get_sfield_from_jstring(const std::string& content, const std::string& field, std::string* out)
{
    cJSON
        *json = NULL,
        *json_id = NULL;

    if ((json = cJSON_Parse(content.c_str())) == NULL) {
        last_error = "it wasn't possible to extract the JSON from string";
        return false;
    }

	const char* c_field = field.c_str();

    if (
        !cJSON_HasObjectItem(json, c_field) ||
        !(json_id = cJSON_GetObjectItem(json, c_field)) ||
        !cJSON_IsString(json_id)
    ) {
        last_error = "it wasn't possible to get the" + field + "from string";
        cJSON_Delete(json);
        return false;
    }

    *out = json_id->valuestring;
    cJSON_Delete(json);
    return true;
}