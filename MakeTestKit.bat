set XDFT8WSJT=..\XDwsjtFT8

set XDFT8KIT=XDft8x86Kit
if exist %XDFT8KIT% rmdir /s /q %XDFT8KIT%
if exist XDft8Test-x86-version.zip del XDft8Test-x86-version.zip
mkdir %XDFT8KIT%
copy Win32\Release\*.dll %XDFT8KIT%
copy Win32\Release\*.exe %XDFT8KIT%
copy %XDFT8WSJT%\binWin32\* %XDFT8KIT%
copy %XDFT8WSJT%\include\COPYING %XDFT8KIT%
pushd %XDFT8KIT%
7z a ../XDft8Test-x86-version.zip *
popd

set XDFT8KIT=XDft8x64Kit
if exist %XDFT8KIT% rmdir /s /q %XDFT8KIT%
if exist XDft8Test-x64-version.zip del XDft8Test-x64-version.zip
mkdir %XDFT8KIT%
copy x64\Release\*.dll %XDFT8KIT%
copy x64\Release\*.exe %XDFT8KIT%
copy %XDFT8WSJT%\binx64\* %XDFT8KIT%
copy %XDFT8WSJT%\include\COPYING %XDFT8KIT%
pushd %XDFT8KIT%
7z a ../XDft8Test-x64-version.zip *
popd
