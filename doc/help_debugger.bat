
:: Enter the full path to the Qt libraries you wish to compile against. 
:: You must specifically point to the "bin" folder in which qmake.exe resides, e.g. C:\qt-4.7.4\bin
:: The Qt directory by itself (C:\qt-4.7.4\) will not work.
@set qt=C:\qt-4.8.4\bin




:: DO *NOT* EDIT BELOW THIS LINE!!!!!!!
:: ###################################################################################

@TITLE ROBOJOURNAL HELP DEBUGGER SCRIPT
@ECHO.
@ECHO Version 1.0 -- 3/23/2013
@ECHO.
@ECHO #############################################################
@ECHO.
@ECHO This script is normally used for debugging purposes since it 
@ECHO allows you to rebuild the help files without having to recompile
@ECHO the rest of the program too. You should run ../win32_compile.bat
@ECHO instead if you want to compile the documentation (and the rest
@ECHO of RoboJournal) for normal use.
@ECHO.
@ECHO This script assumes the following values are true:
@ECHO.
@ECHO.
@ECHO ^* QT ^>^= 4.7.4 is installed at:
@ECHO   [%qt%]
@ECHO.
@ECHO.
@ECHO.
@echo BUILD PATH: %cd%
@echo.
@ECHO #############################################################
@ECHO.

@pause

@path %qt%
@qcollectiongenerator.exe robojournal.qhcp -o robojournal.qhc

@assistant.exe  -collectionFile robojournal.qhc