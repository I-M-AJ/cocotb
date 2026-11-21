
@echo off
rem this file will be overwritten by the project setup script
setlocal
set SIMICS_BASE_PACKAGE=C:\Users\Ajay\AppData\Local\Programs\Simics\simics-7.113.0
if "%SIMICS_PYTHON_PACKAGE%"=="" (
    set SIMICS_PYTHON_PACKAGE=
)
set "SIMICS_PYTHON=%~dp0..\python.bat"
if exist "F:\Anamoly_detection\cocotb\Python_DPIC_RTL\RAL_SIMICS\simics_DML_sv\.package-list" set SIMICS_EPL=--package-list "F:\Anamoly_detection\cocotb\Python_DPIC_RTL\RAL_SIMICS\simics_DML_sv\.package-list"
if not exist "F:\Anamoly_detection\cocotb\Python_DPIC_RTL\RAL_SIMICS\simics_DML_sv\.package-list" set SIMICS_EPL=
"C:\Users\Ajay\AppData\Local\Programs\Simics\simics-7.113.0\bin\list-targets.bat" %SIMICS_EPL% --project "F:\Anamoly_detection\cocotb\Python_DPIC_RTL\RAL_SIMICS\simics_DML_sv" %*
