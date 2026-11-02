@echo off
chcp 65001 >nul
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\Install-RaceBearSkill.ps1" %*
set "RC=%ERRORLEVEL%"
echo.
pause
exit /b %RC%
