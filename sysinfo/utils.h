#pragma once

#include <string>

std::wstring ToWString(const std::string& str);
std::string ToString(const std::wstring& wstr);
void StripWString(std::wstring* str);
void StripString(std::string* str);