md ..\Lib\Win32Debug
md ..\Lib\Win32Release
md ..\Lib\Win32Profile
md ..\Lib\x64Debug
md ..\Lib\x64Release
md ..\Lib\x64Profile

set platform=x64
set configuration=Debug

echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\bzip2.lib ..\Lib\%platform%%configuration%\bzip2.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\bzip2.pdb ..\Lib\%platform%%configuration%\bzip2.pdb
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\lzma.lib ..\Lib\%platform%%configuration%\lzma.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\lzma.pdb ..\Lib\%platform%%configuration%\lzma.pdb
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\ZipLib.lib ..\Lib\%platform%%configuration%\ZipLib.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\ZipLib.pdb ..\Lib\%platform%%configuration%\ZipLib.pdb
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\zlib.lib ..\Lib\%platform%%configuration%\zlib.lib
echo f | xcopy /D /y ziplib\Bin\%platform%\%configuration%\zlib.pdb ..\Lib\%platform%%configuration%\zlib.pdb

echo f | xcopy /D /y assimp\lib\%configuration%\assimp-vc143-mtd.lib ..\Lib\%platform%%configuration%\assimp-vc143-mtd.lib
echo f | xcopy /D /y assimp\bin\%configuration%\assimp-vc143-mtd.pdb ..\Lib\%platform%%configuration%\assimp-vc143-mtd.pdb
echo f | xcopy /D /y assimp\bin\%configuration%\assimp-vc143-mtd.dll ..\..\..\dist\%platform%\%configuration%\assimp-vc143-mtd.dll
echo f | xcopy /D /y assimp\lib\%configuration%\assimp-vc143-mt.lib ..\Lib\%platform%%configuration%\assimp-vc143-mtd.lib
echo f | xcopy /D /y assimp\bin\%configuration%\assimp-vc143-mt.pdb ..\Lib\%platform%%configuration%\assimp-vc143-mtd.pdb
echo f | xcopy /D /y assimp\bin\%configuration%\assimp-vc143-mt.dll ..\..\..\dist\%platform%\%configuration%\assimp-vc143-mtd.dll

echo f | xcopy /D /y tmxlite\tmxlite\build\%platform%\%configuration%Static\libtmxlite-s-d.lib ..\Lib\%platform%%configuration%\libtmxlite.lib
echo f | xcopy /D /y tmxlite\tmxlite\build\%platform%\%configuration%Static\libtmxlite-s-d.pdb ..\Lib\%platform%%configuration%\libtmxlite.pdb

