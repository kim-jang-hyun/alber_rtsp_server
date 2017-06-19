if "%1" == "-vc90_dbg" goto VC90_DEBUG
if "%1" == "-vc90_rel" goto VC90_RELEASE
if "%1" == "-vc140_dbg" goto VC140_DEBUG
if "%1" == "-vc140_rel" goto VC140_RELEASE
goto QUIT

:VC90_DEBUG
copy "..\3rd_party\ffmpeg\bin\*.dll" ".\bin\vc90\dbg"
copy "..\video_sample\*.*" ".\bin\vc90\dbg"
goto QUIT

:VC90_RELEASE
copy "..\3rd_party\ffmpeg\bin\*.dll" ".\bin\vc90\rel"
copy "..\video_sample\*.*" ".\bin\vc90\rel"
goto QUIT

:VC140_DEBUG
copy "..\3rd_party\ffmpeg\bin\*.dll" ".\bin\vc140\dbg"
copy "..\video_sample\*.*" ".\bin\vc140\dbg"
goto QUIT

:VC140_RELEASE
copy "..\3rd_party\ffmpeg\bin\*.dll" ".\bin\vc140\rel"
copy "..\video_sample\*.*" ".\bin\vc140\rel"
goto QUIT

:QUIT