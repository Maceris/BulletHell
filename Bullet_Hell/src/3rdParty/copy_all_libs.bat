md ..\Lib\Win32Debug
md ..\Lib\Win32Release
md ..\Lib\Win32Profile
md ..\Lib\x64Debug
md ..\Lib\x64Release
md ..\Lib\x64Profile

set platform=x64
set configuration=Debug

echo f | xcopy /D /y glfw-3.3.8\msvc\src\%configuration%\glfw3.lib ..\Lib\%platform%%configuration%\glfw3.lib
echo f | xcopy /D /y glfw-3.3.8\msvc\src\%configuration%\glfw3.pdb ..\Lib\%platform%%configuration%\glfw3.pdb

echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\bzip2.lib ..\Lib\%platform%%configuration%\bzip2.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\bzip2.pdb ..\Lib\%platform%%configuration%\bzip2.pdb
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\lzma.lib ..\Lib\%platform%%configuration%\lzma.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\lzma.pdb ..\Lib\%platform%%configuration%\lzma.pdb
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\ZipLib.lib ..\Lib\%platform%%configuration%\ZipLib.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\ZipLib.pdb ..\Lib\%platform%%configuration%\ZipLib.pdb
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\zlib.lib ..\Lib\%platform%%configuration%\zlib.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\zlib.pdb ..\Lib\%platform%%configuration%\zlib.pdb

