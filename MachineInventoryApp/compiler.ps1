Write-Output Compilando MachineInventoryApp...

cd src
g++ -c controllers/appcontroller.cpp -o ../build/obj/appcontroller.o -I../include {wx-config --cxxflags}
if %ERRORLEVEL% NEQ 0 goto :ERRO

g++ -c models/appmodel.cpp -o ../build/obj/appmodel.o -I../include `wx-config --cxxflags`
if %ERRORLEVEL% NEQ 0 goto :ERRO

g++ -c views/appview.cpp -o ../build/obj/appview.o -I../include `wx-config --cxxflags`
g++ -c views/app.cpp -o ../build/obj/app.o -I../include `wx-config --cxxflags`
g++ -c views/forms/authform.cpp -o ../build/obj/authform.o -I../include `wx-config --cxxflags`

g++ -c main.cpp -o ../build/obj/main.o -I../include `wx-config --cxxflags`

cd ../build/obj
g++ resource.res appcontroller.o appmodel.o appview.o app.o authform.o main.o -o ../MachineInventoryApp.exe `wx-config --libs` -mwindows

../MachineInventoryApp.exe

