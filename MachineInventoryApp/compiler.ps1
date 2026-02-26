#!/usr/bin/env powershell

# PATH
$PATH_BUILD = "build"
$PATH_OBJ = "$PATH_BUILD/obj"
$PATH_SOURCE = "src"
$PATH_MINGW64 = "C:/msys64/mingw64"

# FILE
$FILE_EXE = "$PATH_BUILD/MachineInventoryApp.exe"
$FILE_RSC = "$PATH_OBJ/resource.res"

# FLAGS
$FLAGS_EXE = @("-mwindows", "-L$PATH_MINGW64/lib", "-lwx_mswu_xrc-3.2", "-lwx_mswu_html-3.2", "-lwx_mswu_qa-3.2", "-lwx_mswu_core-3.2", "-lwx_baseu_xml-3.2", "-lwx_baseu_net-3.2", "-lwx_baseu-3.2")
$FLAGS_INCLUDE = @("-I$PATH_SOURCE", "-I$PATH_MINGW64/lib/wx/include/msw-unicode-3.2", "-I$PATH_MINGW64/include/wx-3.2", "-DWXUSINGDLL", "-D__WXMSW__")

# Others
$SOURCES = @("controllers/appcontroller.cpp", "models/appmodel.cpp", "views/appview.cpp", "views/app.cpp", "views/forms/authform.cpp", "main.cpp")
$SOURCES_OUT = @("$PATH_OBJ/appcontroller.o", "$PATH_OBJ/appmodel.o", "$PATH_OBJ/appview.o", "$PATH_OBJ/app.o", "$PATH_OBJ/authform.o", "$PATH_OBJ/main.o")

if (!(Test-Path $PATH_BUILD)) {
    mkdir $PATH_BUILD
}

if (!(Test-Path $PATH_OBJ)) {
    mkdir $PATH_OBJ
}

if (!(Test-Path $FILE_RSC)) {
    Copy-Item $PATH_SOURCE/resource.res $FILE_RSC
}

$build_exe_required = 0
for ($i = 0; $i -lt $SOURCES.Count; $i++) {
    $src = $PATH_SOURCE + "/" + $SOURCES[$i]
    $out = $SOURCES_OUT[$i]
    $out_last_write_time = ""

    if (Test-Path $out) {
        $out_last_write_time = (Get-Item $out).LastWriteTime
    }

    if ($out_last_write_time -gt (Get-Item $src).LastWriteTime) {
        # Write-Output "$src up-to-date"
        continue
    }

    $build_exe_required = 1
    
    Write-Output "building $src..."
    g++ -c $src -o $out @FLAGS_INCLUDE
}

if ($build_exe_required -or !(Test-Path $PATH_OBJ)) {
    g++ $FILE_RSC @SOURCES_OUT -o $FILE_EXE $FLAGS_EXE
} else {
    Write-Output "EXE up-to-date"
}

Start-Process $FILE_EXE
