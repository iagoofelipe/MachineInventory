#pragma once

#include <string>

void configure_terminal();
std::wstring ToWString(const std::string& str);
std::string ToString(const std::wstring& wstr);