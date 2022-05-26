@echo off

REM Command to use in Notepad++: cmd /k cd /d $(CURRENT_DIRECTORY) & (if not exist "make.bat" cd ..) & make.bat

REM Create necessary directories
if not exist "obj" mkdir "obj"
if not exist "obj\Debug" mkdir "obj\Debug"
if not exist "bin" mkdir "bin"
if not exist "bin\Debug" mkdir "bin\Debug"

REM Compiles project. If any file yields an error, quits compilation. 
g++.exe -Wall -c -g3 src\digits.cpp -o obj\Debug\digits.o && ^
g++.exe -Wall -c -g3 src\canvas.cpp -o obj\Debug\canvas.o && ^
g++.exe -Wall -c -g3 src\network.cpp -o obj\Debug\network.o && ^
g++.exe -static -static-libgcc -static-libstdc++ -o "bin\Debug\Digits.exe" obj\Debug\digits.o obj\Debug\canvas.o obj\Debug\network.o
if %errorlevel% neq 0 (exit /b %errorlevel%)

bin\Debug\Digits.exe data/train-images.idx3-ubyte data/train-labels.idx1-ubyte data/test-images.idx3-ubyte data/test-labels.idx1-ubyte
if %errorlevel% neq 0 (echo [91mExecution error: %errorlevel%[0m & exit /b %errorlevel%)