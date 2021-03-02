mkdir .\External

# Download x86_64-Windows-MSVC dependencies
curl --output .\External\x86_64-Windows-MSVC.zip --url 192.168.1.66:8090/0.1/Downloads/x86_64-Windows-MSVC.zip

# Download x86_64-Windows-MSVC dependencies
curl --output .\External\x86_64-Windows-MinGW.zip --url 192.168.1.66:8090/0.1/Downloads/x86_64-Windows-MinGW.zip

# Download x86_64-Linux-GCC dependencies
curl --output .\External\x86_64-Linux-GCC.zip --url 192.168.1.66:8090/0.1/Downloads/x86_64-Linux-GCC.zip

# Unzip the dependencies
tar -xf .\External\x86_64-Windows-MSVC.zip -C .\External
tar -xf .\External\x86_64-Windows-MinGW.zip -C .\External
tar -xf .\External\x86_64-Linux-GCC.zip -C .\External

# Delete the downloaded zips
rm -f .\External\x86_64-Windows-MSVC.zip
rm -f .\External\x86_64-Windows-MinGW.zip
rm -f .\External\x86_64-Linux-GCC.zip
