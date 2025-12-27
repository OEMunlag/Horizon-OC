@echo off
setlocal

if not exist dist mkdir dist
set SRC=Source\Atmosphere\stratosphere\loader\source\oc
set DEST=build\stratosphere\loader\source\oc

if not exist dist\atmosphere\kips mkdir dist\atmosphere\kips
if not exist %DEST% mkdir %DEST%

xcopy /E /I /Y "%SRC%\*" "%DEST%\"

cd build\stratosphere\loader || exit /b 1
make -j %NUMBER_OF_PROCESSORS%
hactool -t kip1 out\nintendo_nx_arm64_armv8a\release\loader.kip --uncompress=hoc.kip
cd ..\..\..\

if not exist dist\atmosphere\kips mkdir dist\atmosphere\kips
copy /Y build\stratosphere\loader\hoc.kip dist\atmosphere\kips\hoc.kip

cd Source\sys-clk
if not exist dist mkdir dist
build.bat
xcopy /E /I /Y dist\* ..\..\dist\

cd ..\..

cd Source\Horizon-OC-Monitor
make -j %NUMBER_OF_PROCESSORS%
if not exist ..\..\dist\switch\.overlays mkdir ..\..\dist\switch\.overlays
copy /Y Horizon-OC-Monitor.ovl ..\..\dist\switch\.overlays\Horizon-OC-Monitor.ovl

cd ..\..

set ROOT=build
set PATCHES=Atmosphere-Patches

copy /Y "%PATCHES%\secmon_memory_layout.hpp" "%ROOT%\libraries\libexosphere\include\exosphere\secmon\"
copy /Y "%PATCHES%\secmon_emc_access_table_data.inc" "%ROOT%\exosphere\program\source\smc\"
copy /Y "%PATCHES%\secmon_define_emc_access_table.inc" "%ROOT%\exosphere\program\source\smc\"
copy /Y "%PATCHES%\secmon_smc_register_access.cpp" "%ROOT%\exosphere\program\source\smc\"

cd build\exosphere
make -j %NUMBER_OF_PROCESSORS%

cd out\nintendo_nx_arm64_armv8a\release
if not exist ..\..\..\..\..\dist mkdir ..\..\..\..\..\dist
copy /Y exosphere.bin ..\..\..\..\..\dist\secmon_emc_patch.bin

endlocal
