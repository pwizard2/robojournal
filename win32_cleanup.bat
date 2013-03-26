

:: Enter the full path to MinGW toolkit. It usually installs to C:\MinGW\bin
@set mingw=C:\MinGW\bin


:: DO *NOT* EDIT BELOW THIS LINE!!!!!!!
:: ###################################################################################

@TITLE ROBOJOURNAL BUILD CLEAN SCRIPT FOR WINDOWS 2000/XP/VISTA/7
@ECHO.
@ECHO Version 1.1 -- 3/20/2013

@ECHO.
@ECHO #############################################################
@ECHO.
@ECHO This script assumes the following values are true:
@ECHO.
@ECHO ^* 32-bit MinGW (compiler) toolkit is installed at:
@ECHO   [%mingw%]
@ECHO.
@ECHO #############################################################
@ECHO.
@ECHO WARNING: This script erases ALL compiled files in this 
@ECHO          folder and its sub-directories, returning all source
@ECHO          files to their original state. Proceed with caution^!^!^!
@ECHO.
@PAUSE
@ECHO.
@path %mingw%
mingw32-make distclean
RMDIR /s /q debug
RMDIR /s /q release
DEL *.Release *.Debug *.pro.user *.qch

@ECHO.
@ECHO #############################################################
@ECHO Clean-up operation completed!
@PAUSE
EXIT

