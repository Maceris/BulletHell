md ..\Lib\Win32Debug
md ..\Lib\Win32Release
md ..\Lib\Win32Profile
md ..\Lib\x64Debug
md ..\Lib\x64Release
md ..\Lib\x64Profile

echo f | xcopy /D /y glfw-3.3.8\msvc\src\Debug\glfw3.lib ..\Lib\x64Debug\glfw3.lib
echo f | xcopy /D /y glfw-3.3.8\msvc\src\Debug\glfw3.pdb ..\Lib\x64Debug\glfw3.pdb
