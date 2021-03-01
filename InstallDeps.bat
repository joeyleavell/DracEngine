mkdir .\External

REM Download x86_64-Windows-MSVC dependencies
curl --output .\External\x86_64-Windows-MSVC.zip --url 192.168.1.66:8090/0.1/Downloads/x86_64-Windows-MSVC.zip

REM Download x86_64-Linux-GCC dependencies
curl --output .\External\x86_64-Linux-GCC.zip --url 192.168.1.66:8090/0.1/Downloads/x86_64-Linux-GCC.zip

REM Unzip the dependencies
tar -xf .\External\x86_64-Windows-MSVC.zip -C .\External
tar -xf .\External\x86_64-Linux-GCC.zip -C .\External

REM Delete the downloaded zips
del /f .\External\x86_64-Windows-MSVC.zip
del /f .\External\x86_64-Linux-GCC.zip