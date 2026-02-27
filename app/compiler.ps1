#!/usr/bin/env powershell

[xml] $xml = Get-Content "project.xml"
$project = $xml.project

# PATH
$PATH_BUILD = "build"
$PATH_OBJ = "$PATH_BUILD\obj"
$PATH_SOURCE = $project.sources.root
$PATH_MINGW64 = "C:\msys64\mingw64"

# FILE
$FILE_EXE = "$PATH_BUILD\" + $project.name + ".exe"
$FILE_RSC = "$PATH_SOURCE\resource.res"

# FLAGS
$FLAGS_SYSINFO = @("-L../sysinfo/lib", "-lsysinfo", "-lwbemuuid", "-lcurl", "-loleaut32", "-lole32")
$FLAGS_WX = @("-lpthread", "-mwindows", "-L$PATH_MINGW64/lib", "-lwx_mswu_xrc-3.2", "-lwx_mswu_html-3.2", "-lwx_mswu_qa-3.2", "-lwx_mswu_core-3.2", "-lwx_baseu_xml-3.2", "-lwx_baseu_net-3.2", "-lwx_baseu-3.2")
$FLAGS_INCLUDE = @("-I../sysinfo/include", "-Iinclude", "-I$PATH_MINGW64/lib/wx/include/msw-unicode-3.2", "-I$PATH_MINGW64/include/wx-3.2", "-DWXUSINGDLL", "-D__WXMSW__")

# Others
$SOURCES = $project.sources.file | ForEach-Object { $_.path -replace "/", "\" }
$SOURCES_OUT = $SOURCES | ForEach-Object { "$PATH_OBJ\" + ($_ -replace "\\", "_" -replace "\.(cpp|cxx|c|cc)$", ".o") }

if (!(Test-Path $PATH_BUILD)) {
    mkdir $PATH_BUILD
}

if (!(Test-Path $PATH_OBJ)) {
    mkdir $PATH_OBJ
}

$build_exe_required = 0
for ($i = 0; $i -lt $SOURCES.Count; $i++) {
    $src = $PATH_SOURCE + "\" + $SOURCES[$i]
    $out = $SOURCES_OUT[$i]
    $out_last_write_time = ""

    if (Test-Path $out) {
        $out_last_write_time = (Get-Item $out).LastWriteTime
    }

    if ($out_last_write_time -gt (Get-Item $src).LastWriteTime) {
        continue
    }

    $build_exe_required = 1
    
    Write-Output "building '$src' in '$out'..."
    g++ -c $src -o $out @FLAGS_INCLUDE
}

if ($build_exe_required -or !(Test-Path $FILE_EXE)) {
    g++ $FILE_RSC @SOURCES_OUT -o $FILE_EXE @FLAGS_WX @FLAGS_SYSINFO
} else {
    Write-Output "EXE up-to-date"
}

Start-Process $FILE_EXE
