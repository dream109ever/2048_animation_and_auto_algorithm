@echo off
setlocal

:: ================== 用户配置区域 ==================
:: 请根据你的实际路径修改以下两项
set RAYLIB_DIR=D:\raylib-4.0.0_win64_mingw-w64
set MINGW_DIR=D:\MinGW64\MinGW64
:: ================================================

:: 设置编译器和标志
set CC=%MINGW_DIR%\bin\gcc.exe
set CFLAGS=-I"%RAYLIB_DIR%\include" -I"%MINGW_DIR%\include" -g
set LDFLAGS=-L"%RAYLIB_DIR%\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

:: 编译资源文件
set WINDRES=%MINGW_DIR%\bin\windres.exe
%WINDRES% -i 2048_private.rc -o 2048_private.o

:: 源文件列表（请根据实际情况增删）
set SOURCES=main.c animation.c ui.c background.c ai_mode.c board.c globals.c 2048_private.o
set OUTPUT=2048.exe

echo Compiling...
%CC% %CFLAGS% %SOURCES% -o %OUTPUT% %LDFLAGS%
if %errorlevel% neq 0 (
    echo Compilation failed.
    pause
    exit /b %errorlevel%
)

echo Build successful! Run %OUTPUT%
:: 可选：自动运行
%OUTPUT%
endlocal