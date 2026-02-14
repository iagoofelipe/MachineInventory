#pragma once
#include "sysinfo.h"

struct response {
	int status_code = 0;
	std::string body;
};

bool init_console();
void cleanup_console();

void show_machine(const sysinfo::machine& machine);
bool upload_machine(const sysinfo::machine& machine, const wchar_t* cpf = NULL, const wchar_t* title = NULL);