$PATH_LIB = "lib"
$PATH_SRC = "src"
$PATH_OBJ = "$PATH_LIB\obj"

g++ -c $PATH_SRC\cJSON.c -o $PATH_OBJ\cJSON.o -Iinclude/sysinfo
g++ -c $PATH_SRC\server.cpp -o $PATH_OBJ\server.o -Iinclude/sysinfo
g++ -c $PATH_SRC\utils.cpp -o $PATH_OBJ\utils.o -Iinclude/sysinfo
g++ -c $PATH_SRC\sysinfo.cpp -o $PATH_OBJ\sysinfo.o --std=c++20 -Iinclude/sysinfo

ar rcs $PATH_LIB\libsysinfo.a $PATH_OBJ\cJSON.o $PATH_OBJ\server.o $PATH_OBJ\sysinfo.o $PATH_OBJ\utils.o