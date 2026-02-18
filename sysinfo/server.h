#pragma once

#include "sysinfo.h"
#include "cJSON.h"

#include <curl/curl.h>
#include <string>

namespace sysinfo
{
	struct response {
		int status_code = 0;
		std::string content;
	};

	struct user {
		std::string id;
		std::string cpf;
		std::string name;
	};

	class ServerConnection
	{
	public:
		ServerConnection();
		~ServerConnection();

		const char* get_last_error();
		const char* get_token();
		bool is_ready();
		bool has_token();
		bool validate_token();
		bool login(const std::string& cpf, const std::string& password);

		bool get_user(user* u);
		bool get_user(const std::string& cpf_or_id, user* u);
		bool create_new_user(const std::string& cpf, const std::string& name, const std::string& password, std::string* id);
		bool upload_machine(cJSON* json, const char* ownerCpf, const char* machineTitle, std::string* id);
		bool upload_machine(const machine* data, const char* ownerCpf, const char* machineTitle, std::string* id);

	private:
		static const std::string BASE_URL;
		std::string token;
		std::string last_error;

		CURL* curl;


		bool get_request(const std::string& url, response* r);
		bool post_request(const std::string& url, const std::string& data, response* r);

		bool get_sfield_from_jstring(const std::string& content, const std::string& field, std::string* out);
	};
}