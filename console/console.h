#pragma once

#include <string_view>
#include <iostream>

#include <sysinfo/server.h>
#include <sysinfo/utils.h>

void show_help();
bool init_console(const std::wstring_view& command);
void cleanup();

int command_sysinfo();
int command_upload(const wchar_t* cpf, const wchar_t* machineTitle);
int command_auth(const wchar_t* cpf, const wchar_t* password);
int command_user(const wchar_t* cpf);
int command_newuser(const wchar_t* cpf, const wchar_t* name, const wchar_t* password);
int command_validatetoken();