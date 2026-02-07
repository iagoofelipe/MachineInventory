#include "pch.h"
#include "framework.h"

#include "utils.h"

#include <fcntl.h>  // Necessário para _O_U16TEXT
#include <io.h>     // Necessário para _setmode
#include <stdio.h>

void sysinfo::configure_terminal()
{
    int r = _setmode(_fileno(stdout), _O_U16TEXT); // configura UTF-16 para std::wcout
}