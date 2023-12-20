msbuild /p:Configuration=Debug /p:Platform=x64 ziplib\ZipLib.sln
msbuild /p:Configuration=Debug /p:Platform=x64 assimp\Assimp.sln

copy_all_libs
