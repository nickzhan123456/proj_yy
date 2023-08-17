@echo off
echo "使用bat脚原本复制文件夹和文件"
echo.
xcopy ..\..\..\build-MtcControlLibuvcCamera-Desktop_Qt_5_12_4_MinGW_32_bit-Debug\uvcCamera\debug\uvcCamera.exe     ..\..\..\senseengine_lib\bin\win32\debug    /s /e /c /y /h /r 
xcopy ..\..\..\build-MtcControlLibuvcCamera-Desktop_Qt_5_12_4_MinGW_32_bit-Release\uvcCamera\release\uvcCamera.exe ..\..\..\senseengine_lib\bin\win32\release  /s /e /c /y /h /r
pause
