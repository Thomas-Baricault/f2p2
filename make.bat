@ECHO OFF


CD tool

DEL scanner.exe
G++ scanner.cpp -o scanner
scanner

DEL f2p2.exe
G++ -std=c++17 main.cpp -o f2p2 -lcurl -lmariadb

@REM COPY f2p2.exe C:\Users\utilisateur\Desktop\test\f2p2.exe

@REM DEL test.exe
@REM G++ test.cpp -o test
@REM test
