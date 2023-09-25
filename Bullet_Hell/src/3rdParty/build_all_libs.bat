msbuild /p:Configuration=Debug /p:Platform=x64 glfw-3.3.8\msvc\GLFW.sln
msbuild /p:Configuration=Debug /p:Platform=x64 ziplib\ZipLib.sln

copy_all_libs
