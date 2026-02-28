#pragma once
#include <string>

enum unit {
	U_BYTE,
	U_KILOBYTE,
	U_MEGABYTE,
	U_GIGABYTE,
	U_TERABYTE
};

std::wstring ToWString(const std::string& str);
std::string ToString(const std::wstring& wstr);
void StripWString(std::wstring* str);
void StripString(std::string* str);

std::wstring SimplifyMetric(const std::wstring& num, unit u);
std::wstring SimplifyMetric(double num, unit u);
