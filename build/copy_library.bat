if "%1" == "-dbg" goto DEBUG
if "%1" == "-rel" goto RELEASE
goto QUIT

:DEBUG
copy "..\3rd_party\ffmpeg\bin\*.dll" ".\bin\dbg"
copy "..\video_sample\*.*" ".\bin\dbg"
goto QUIT

:RELEASE
copy "..\3rd_party\ffmpeg\bin\*.dll" ".\bin\rel"
copy "..\video_sample\*.*" ".\bin\rel"
goto QUIT

:QUIT