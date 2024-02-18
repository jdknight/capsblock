@echo off
reg delete HKCU\Software\Microsoft\Windows\CurrentVersion\Run ^
    /v "Caps Block" /f >NUL
