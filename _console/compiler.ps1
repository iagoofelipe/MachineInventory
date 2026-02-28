$PATH_BUILD = "build"
$PATH_SRC = "src"
$PATH_OBJ = "$PATH_BUILD\obj"
$FLAGS_INCLUDE = @("-I../sysinfo/include")
$FLAGS_SYSINFO = @("-L../sysinfo/lib", "-lsysinfo", "-lwbemuuid", "-lcurl", "-loleaut32", "-lole32")

g++ -c $PATH_SRC\console.cpp -o $PATH_OBJ\console.o @FLAGS_INCLUDE
g++ -c $PATH_SRC\main.cpp -o $PATH_OBJ\main.o @FLAGS_INCLUDE

g++ -o $PATH_BUILD\console.exe $PATH_OBJ\main.o $PATH_OBJ\console.o @FLAGS_SYSINFO @FLAGS_INCLUDE -municode