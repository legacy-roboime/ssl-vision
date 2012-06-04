:: simple script to call cmake-gui on windows

@echo off

:: check for cmake
cmake-gui --version

:: check if the command was successful
if errorlevel 1 (
	echo.
	echo CMake GUI was not found on your path.
	echo.
	echo.
	echo If you don't have cmake, please read the README.md to see the requirements.
	echo.
	echo.
	echo If you have cmake either add it to your path and try again,
	echo or run it manually and set the following options:
	echo.
	echo 	Where is the source code: %cd%
	echo 	Where to build the binaries: %cd%\build
	echo.
	echo It's also recommended to check Grouped and Advanced if you're having trouble
	echo configuring the dependencies.
	echo.
	echo.
	pause
) else (
	cd build
	start cmake-gui ..
	cd ..
)
