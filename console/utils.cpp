#include "utils.h"

#include <fcntl.h>  // Necessario para _O_U16TEXT
#include <io.h>     // Necessario para _setmode
#include <windows.h>
#include <stdio.h>

void configure_terminal()
{
    _setmode(_fileno(stdin), _O_WTEXT);    // configura UTF-16 para std::wcin
    _setmode(_fileno(stdout), _O_U16TEXT); // configura UTF-16 para std::wcout
}

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