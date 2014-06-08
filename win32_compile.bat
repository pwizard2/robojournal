
:: Enter the full path to the Qt libraries you wish to compile against. 
:: You must specifically point to the "bin" folder in which qmake.exe resides, e.g. C:\qt-4.7.4\bin
:: The Qt directory by itself (C:\qt-4.7.4\) will not work.
@set qt=C:\Qt\qt-everywhere-opensource-src-4.8.5\bin

:: Enter the full path to MinGW toolkit. It usually installs to C:\MinGW\bin
@set mingw=C:\MinGW\mingw32\bin


:: DO *NOT* EDIT BELOW THIS LINE!!!!!!!
:: ###################################################################################

@TITLE ROBOJOURNAL BUILD HELPER SCRIPT FOR WINDOWS 2000/XP/VISTA/7
@ECHO.
@ECHO Version 1.4 -- 4/2/2013
@ECHO.
@ECHO #############################################################
@ECHO.
@ECHO This script assumes the following values are true:
@ECHO.
@ECHO.
@ECHO ^* QT ^>^= 4.7.4 is installed at:
@ECHO   [%qt%]
@ECHO.
@ECHO ^* 32-bit MinGW (compiler) toolkit is installed at:
@ECHO   [%mingw%]
@ECHO.
@ECHO These values MUST correspond to the install locations of Qt and MinGW
@ECHO on your system. Change them if necessary BEFORE running this script!
@echo Open win32_compile.bat in a text editor if you need to make changes. 
@ECHO.
@echo BUILD PATH: %cd%\release
@echo.
@ECHO #############################################################
@ECHO.

@pause

@path %qt%
@qmake robojournal.pro
@path %mingw%

mingw32-make 

strip -s release/robojournal.exe

@path %qt%
::@qhelpgenerator.exe doc/robojournal.qhp -o release/robojournal.qch
::qcollectiongenerator.exe doc/robojournal.qhcp -o release/robojournal.qhc
@qcollectiongenerator.exe doc/robojournal.qhcp -o doc/robojournal.qhc
@move doc\robojournal.qch release
@move doc\robojournal.qhc release
@ECHO.

:: Comment out the next three lines if you want to keep object code (*.o) and MOC-generated code
@ECHO Cleaning up the build ^(release^) folder...
@cd release
@del *.cpp *.o

@ECHO.
@ECHO #############################################################
@ECHO Build process complete!
@ECHO.
@ECHO robojournal.exe will work by itself if the Qt you compiled against 
@ECHO is static-built ^(i.e. integrates all dependencies into the compiled 
@ECHO binary^).
@ECHO.
@ECHO If your Qt is ^*NOT^* static-built, please consult the 
@ECHO ^"Resolving Windows Dependencies^" section in the RoboJournal 
@ECHO Compile^/Installation Guide (compile-instructions.xhtml) for 
@ECHO instructions on how to resolve all necessary dependencies.
@ECHO.
@ECHO Thank you for using RoboJournal. 
@ECHO.
@pause