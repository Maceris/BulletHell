msbuild /p:Configuration=Debug /p:Platform=x64 ziplib\ZipLib.sln
msbuild /p:Configuration=Debug /p:Platform=x64 assimp\Assimp.sln
msbuild /p:Configuration=DebugStatic /p:Platform=x64 tmxlite\tmxlite.sln

copy_all_libs
