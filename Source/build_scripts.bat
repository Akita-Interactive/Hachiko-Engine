@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set current_dir=%cd%
msbuild.exe /m /p:SolutionDir=%cd%\ /t:build /p:configuration=debug /p:platform=x64 "..\Gameplay\Gameplay.vcxproj" 