# g++ AuthOwnerForm.cpp MachineInventoryApp.cpp MainWindow.cpp -I../sysinfo ../sysinfo/cJSON.c ../sysinfo/pch.cpp ../sysinfo/server.cpp ../sysinfo/sysinfo.cpp ../sysinfo/utils.cpp `wx-config --cxxflags --libs` -mwindows --std=c++20 -lwbemuuid -loleaut32 -lole32 -lcurl -o build/MachineInventoryApp.exe

g++ -c ../sysinfo/cJSON.c -o build/obj/cJSON.o
g++ -c ../sysinfo/sysinfo.cpp -o build/obj/sysinfo.o --std=c++20
g++ -c ../sysinfo/server.cpp -o build/obj/server.o
g++ -c ../sysinfo/utils.cpp -o build/obj/utils.o
g++ -c AuthOwnerForm.cpp -o build/obj/AuthOwnerForm.o `wx-config --cxxflags` -I../sysinfo
g++ -c MainWindow.cpp -o build/obj/MainWindow.o `wx-config --cxxflags` -I../sysinfo
g++ -c MachineInventoryApp.cpp -o build/obj/MachineInventoryApp.o `wx-config --cxxflags` -I../sysinfo

g++ build/obj/AuthOwnerForm.o build/obj/MachineInventoryApp.o build/obj/MainWindow.o build/obj/cJSON.o build/obj/server.o build/obj/sysinfo.o build/obj/utils.o -I../sysinfo `wx-config --cxxflags --libs` -mwindows -lwbemuuid -loleaut32 -lole32 -lcurl -o build/MachineInventoryApp.exe
