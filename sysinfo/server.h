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
		int rules = 0;
	};

	enum USER_RULE {
		CREATE_USERS = 1 << 1,
		QUERY_OTHER_USERS = 1 << 2,
		ADD_MACHINE = 1 << 3,
		UPDATE_USER_RULES = 1 << 4,
	};

	class ServerAPI
	{
	public:
		ServerAPI();
		~ServerAPI();

		bool Initialize();
		const char* GetLastError();
		bool NoConnectionInLastRequest() const { return conn_refused_last_request; }
		bool TestConnection();

		bool HasToken();
		bool ValidateToken();
		void ClearToken();
		bool Auth(const std::string& cpf, const std::string& password);

		bool GetUser(user* u, const std::string& cpf_or_id = "");
		bool CreateNewUser(const std::string& cpf, const std::string& name, const std::string& password, std::string* id = nullptr);
		bool UploadMachine(cJSON* json, const char* ownerCpf, const char* machineTitle, std::string* id = nullptr);
		bool UploadMachine(const machine* data, const char* ownerCpf, const char* machineTitle, std::string* id = nullptr);
		bool GetMachine(const char* mac, machine* m);

	private:
		static const std::string BASE_URL;

		std::string FILE_TOKEN;
		std::string token;
		std::string last_error;
		CURL* curl;
		bool conn_refused_last_request;

		bool getRequest(const std::string& url, response* r);
		bool postRequest(const std::string& url, const std::string& data, response* r);
		bool getSfieldFromJstring(const std::string& content, const std::string& field, std::string* out);
	};
}