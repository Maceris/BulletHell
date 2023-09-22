md ..\Lib\Win32Debug
md ..\Lib\Win32Release
md ..\Lib\Win32Profile
md ..\Lib\x64Debug
md ..\Lib\x64Release
md ..\Lib\x64Profile

echo f | xcopy /D /y ziplib\Bin\x64\Debug\bzip2.lib ..\Lib\x64Debug\bzip2.lib
echo f | xcopy /D /y ziplib\Bin\x64\Debug\bzip2.pdb ..\Lib\x64Debug\bzip2.pdb
echo f | xcopy /D /y ziplib\Bin\x64\Debug\lzma.lib ..\Lib\x64Debug\lzma.lib
echo f | xcopy /D /y ziplib\Bin\x64\Debug\lzma.pdb ..\Lib\x64Debug\lzma.pdb
echo f | xcopy /D /y ziplib\Bin\x64\Debug\ZipLib.lib ..\Lib\x64Debug\ZipLib.lib
echo f | xcopy /D /y ziplib\Bin\x64\Debug\ZipLib.pdb ..\Lib\x64Debug\ZipLib.pdb
echo f | xcopy /D /y ziplib\Bin\x64\Debug\zlib.lib ..\Lib\x64Debug\zlib.lib
echo f | xcopy /D /y ziplib\Bin\x64\Debug\zlib.pdb ..\Lib\x64Debug\zlib.pdb
