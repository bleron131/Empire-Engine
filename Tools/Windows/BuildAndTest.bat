@echo off
rem Empire Engine - double-click to build the editor and run the automated tests.
rem The report is written to Saved\EmpireReport\Report.txt
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0BuildAndTest.ps1" %*
echo.
pause
