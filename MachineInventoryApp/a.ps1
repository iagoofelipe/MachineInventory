#!/usr/bin/env powershell

Write-Output "Compilando MachineInventoryApp..."

$PATH_BUILD = "$PSScriptRoot/build"
$PATH_OBJ = "$PSScriptRoot/build/obj"
$PATH_SOURCE = "$PSScriptRoot/src"
$LIBS = "-LC:/msys64/mingw64/lib  -lwx_mswu_xrc-3.2 -lwx_mswu_html-3.2 -lwx_mswu_qa-3.2 -lwx_mswu_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2"
$INCLUDE = "-I$PATH_SOURCE -IC:/msys64/mingw64/lib/wx/include/msw-unicode-3.2 -IC:/msys64/mingw64/include/wx-3.2 -DWXUSINGDLL -D__WXMSW__"

if (!(Test-Path $PATH_BUILD)) {
    mkdir $PATH_BUILD
}

if (!(Test-Path $PATH_OBJ)) {
    mkdir $PATH_OBJ
    Copy-Item $PATH_SOURCE/resource.res $PATH_OBJ/resource.res
}

g++ -c $PATH_SOURCE/controllers/appcontroller.cpp -o $PATH_OBJ/appcontroller.o $INCLUDE
