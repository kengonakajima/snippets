@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set VCPKG_ROOT=C:\vcpkg
set STATIC_INCLUDE=%VCPKG_ROOT%\installed\x64-windows-static\include
set STATIC_LIB=%VCPKG_ROOT%\installed\x64-windows-static\lib

echo Compiling with static libraries...
echo Include: %STATIC_INCLUDE%
echo Library: %STATIC_LIB%

cl /nologo /W3 /O2 /MT /D_WIN32 /DWIN32 /D_WINDOWS /DCURL_STATICLIB /I"%STATIC_INCLUDE%" proxy_detect_windows.c /link /LIBPATH:"%STATIC_LIB%" libcurl.lib duktape.lib zlib.lib winhttp.lib wininet.lib ws2_32.lib wldap32.lib crypt32.lib normaliz.lib advapi32.lib user32.lib bcrypt.lib /OUT:proxy_detect_windows_static.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Testing...
    proxy_detect_windows_static.exe https://www.google.com
) else (
    echo Build failed!
    if exist "%STATIC_LIB%\libcurl.lib" (
        echo libcurl.lib: Found
    ) else (
        echo libcurl.lib: NOT FOUND
    )
    if exist "%STATIC_LIB%\duktape.lib" (
        echo duktape.lib: Found
    ) else (
        echo duktape.lib: NOT FOUND
    )
)

pause
