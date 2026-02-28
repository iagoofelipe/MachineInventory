#include "utils.h"

#include <windows.h>
#include <string>

std::wstring ToWString(const std::string& str) {
    if (str.empty()) return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string ToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void StripWString(std::wstring* str) {
    const wchar_t* whitespace = L" \t\n\r\f\v";
    size_t last = str->find_last_not_of(whitespace);

    if (last != std::wstring::npos)
        str->erase(last + 1);
    else
        str->clear();
}

void StripString(std::string* str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t last = str->find_last_not_of(whitespace);

    if (last != std::string::npos)
        str->erase(last + 1);
    else
        str->clear();
}

std::wstring SimplifyMetric(const std::wstring& num, unit u) {
    return SimplifyMetric(std::stod(num), u);
}

std::wstring SimplifyMetric(double num, unit u) {
    const wchar_t* units[] = { L"B", L"KB", L"MB", L"GB", L"TB" };
    int unit_index = static_cast<int>(u);

    while (num >= 1024 && unit_index < 4) {
        num /= 1024;
        unit_index++;
    }

    wchar_t buffer[50];
    swprintf(buffer, 50, L"%d %ls", (int)num, units[unit_index]);

    return buffer;
}