#ifndef SYSINFOSERVER_H
#define SYSINFOSERVER_H

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

	class ServerAPI
	{
	public:
		ServerAPI();
		~ServerAPI();

		bool Initialize();
		const char* GetLastError();
		bool HasToken();
		bool ValidateToken();
		void ClearToken();
		bool Auth(const std::string& cpf, const std::string& password);

		bool GetUser(user* u);
		bool GetUser(const std::string& cpf_or_id, user* u);
		bool CreateNewUser(const std::string& cpf, const std::string& name, const std::string& password, std::string* id);
		bool UploadMachine(cJSON* json, const char* ownerCpf, const char* machineTitle, std::string* id);
		bool UploadMachine(const machine* data, const char* ownerCpf, const char* machineTitle, std::string* id);

	private:
		static const std::string BASE_URL;
		std::string token;
		std::string last_error;

		CURL* curl;

		bool getRequest(const std::string& url, response* r);
		bool postRequest(const std::string& url, const std::string& data, response* r);
		bool getSfieldFromJstring(const std::string& content, const std::string& field, std::string* out);
	};
}

#endif