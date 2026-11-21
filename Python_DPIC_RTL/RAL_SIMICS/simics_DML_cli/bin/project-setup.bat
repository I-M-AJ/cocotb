@echo off
rem this file will be overwritten by the project setup script
setlocal
set SIMICS_BASE_PACKAGE=C:\Users\Ajay\AppData\Local\Programs\Simics\simics-7.113.0
if "%SIMICS_PYTHON_PACKAGE%"=="" (
    set SIMICS_PYTHON_PACKAGE=
)
set "SIMICS_PYTHON=C:\Users\Ajay\AppData\Local\Python\pythoncore-3.15-64\python.exe"
"C:\Users\Ajay\AppData\Local\Programs\Simics\simics-7.113.0\bin\project-setup.bat" --project "F:\Anamoly_detection\cocotb\Python_DPIC_RTL\RAL_SIMICS\simics_DML_cli" %*
