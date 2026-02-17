#pragma once

#include <string>

std::wstring ToWString(const std::string& str);
std::string ToString(const std::wstring& wstr);