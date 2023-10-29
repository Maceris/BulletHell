msbuild /p:Configuration=Debug /p:Platform=x64 glfw-3.3.8\msvc\GLFW.sln
msbuild /p:Configuration=Debug /p:Platform=x64 ziplib\ZipLib.sln
msbuild /p:Configuration=DebugStatic /p:Platform=x64 tmxlite\tmxlite.sln

copy_all_libs
