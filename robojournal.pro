#-------------------------------------------------
#
# Project created by QtCreator 2011-08-31T19:59:51
#
#-------------------------------------------------

# Do *NOT* edit below this line!
######################################################

# Declaration of all Qt modules that should be built into the binary.
QT += core gui sql webkit network

TARGET = robojournal
TEMPLATE = app

# Relative paths to all source files, headers, and UI files that need to be built.
SOURCES += main.cpp\
        ui/mainwindow.cpp \
	core/buffer.cpp \
	ui/editor.cpp \
	ui/dblogin.cpp \
	sql/mysqlcore.cpp \
	sql/psqlcore.cpp \
	sql/sqlitecore.cpp \
	ui/aboutrj.cpp \
	ui/firstrun.cpp \
	ui/tagger.cpp \
	ui/journalselector.cpp \
	ui/entryexporter.cpp \
	ui/newconfig.cpp \
	core/settingsmanager.cpp \
	ui/configurationgeneral.cpp \
	ui/configurationjournal.cpp \
	ui/configurationmysql.cpp \
	ui/configurationappearance.cpp \
	ui/configurationeditor.cpp \
	ui/configurationexport.cpp \
	ui/exportpreview.cpp \
	sql/sqlshield.cpp \
	ui/tagreminder.cpp \
        ui/newjournalcreator.cpp \
        ui/mysqljournalpage.cpp \
        ui/sqlitejournalpage.cpp \
        ui/journalcreatorcoverpage.cpp \
        ui/configurationsqlite.cpp \
        ui/editortagmanager.cpp \
        core/taggingshared.cpp \
        core/htmlcore.cpp \
        ui/permissionmanager.cpp \
        ui/outputpane.cpp \
        core/favoritecore.cpp \
        ui/exportsingle.cpp \
        ui/exportmulti.cpp \
        ui/exportcreatedump.cpp \
        ui/customwords.cpp \
        core/hunspell/spellchecker.cpp \
        core/hunspell/utf_info.cxx \
        core/hunspell/suggestmgr.cxx \
        core/hunspell/replist.cxx \
        core/hunspell/phonet.cxx \
        core/hunspell/hunzip.cxx \
        core/hunspell/hunspell.cxx \
        core/hunspell/hashmgr.cxx \
        core/hunspell/filemgr.cxx \
        core/hunspell/dictmgr.cxx \
        core/hunspell/csutil.cxx \
        core/hunspell/affixmgr.cxx \
        core/hunspell/affentry.cxx \
        core/hunspell/ctextcheckeredit.cpp \
        core/hunspell/ctextsyntaxhighlighter.cpp


HEADERS  += ui/mainwindow.h \
	core/buffer.h \
	ui/editor.h \
	ui/dblogin.h \
	sql/mysqlcore.h \
	sql/psqlcore.h \
	sql/sqlitecore.h \
	ui/aboutrj.h \
	ui/firstrun.h \
	ui/tagger.h \
	ui/journalselector.h \
	ui/entryexporter.h \
	ui/newconfig.h \
	core/settingsmanager.h \
	ui/configurationgeneral.h \
	ui/configurationjournal.h \
	ui/configurationmysql.h \
	ui/configurationappearance.h \
	ui/configurationeditor.h \
	ui/configurationexport.h \
	ui/exportpreview.h \
	sql/sqlshield.h \
	ui/tagreminder.h \
        ui/newjournalcreator.h \
        ui/mysqljournalpage.h \
        ui/sqlitejournalpage.h \
        ui/journalcreatorcoverpage.h \
        ui/configurationsqlite.h \
        ui/editortagmanager.h \
        core/taggingshared.h \
        core/htmlcore.h \
	ui/permissionmanager.h \
	ui/outputpane.h \
	core/favoritecore.h \
	ui/exportsingle.h \
	ui/exportmulti.h \
	ui/exportcreatedump.h \
        ui/customwords.h \
        core/hunspell/spellchecker.h \
        core/hunspell/w_char.hxx \
        core/hunspell/suggestmgr.hxx \
        core/hunspell/replist.hxx \
        core/hunspell/phonet.hxx \
        core/hunspell/langnum.hxx \
        core/hunspell/hunzip.hxx \
        core/hunspell/hunvisapi.h \
        core/hunspell/hunspell.hxx \
        core/hunspell/hunspell.h \
        core/hunspell/htypes.hxx \
        core/hunspell/hashmgr.hxx \
        core/hunspell/filemgr.hxx \
        core/hunspell/dictmgr.hxx \
        core/hunspell/csutil.hxx \
        core/hunspell/baseaffix.hxx \
        core/hunspell/atypes.hxx \
        core/hunspell/affixmgr.hxx \
        core/hunspell/affentry.hxx \
        core/hunspell/ctextcheckeredit.h \
        core/hunspell/ctextsyntaxhighlighter.h


FORMS    += ui/mainwindow.ui \
	ui/editor.ui \
	ui/dblogin.ui \
	ui/aboutrj.ui \
	ui/firstrun.ui \
	ui/tagger.ui \
	ui/journalselector.ui \
	ui/entryexporter.ui \
	ui/newconfig.ui \
	ui/configurationgeneral.ui \
	ui/configurationjournal.ui \
	ui/configurationmysql.ui \
	ui/configurationappearance.ui \
	ui/configurationeditor.ui \
	ui/configurationexport.ui \
	ui/exportpreview.ui \
	ui/tagreminder.ui \
        ui/newjournalcreator.ui \
        ui/mysqljournalpage.ui \
        ui/sqlitejournalpage.ui \
        ui/journalcreatorcoverpage.ui \
        ui/configurationsqlite.ui \
        ui/editortagmanager.ui \
	ui/permissionmanager.ui \
	ui/outputpane.ui \
	ui/exportsingle.ui \
	ui/exportmulti.ui \
	ui/exportcreatedump.ui \
        ui/customwords.ui


# Declaration of images and other to-be-embedded resources.
RESOURCES  = images.qrc

# The RC_File (resource file) sets the program icon for the Win32 executable robojournal.exe.
RC_FILE = icon.rc

# This block contains all Linux/Unix-specific build instructions.
unix {

    # The package block sets a different install path for package builds.
    package {
        message(Creating a package-friendly build...)
	target.path = /usr/bin
	
    }
    else{
	message(Creating a regular build...)
	target.path = /usr/local/bin

    }

    # Tweak for development/debugging: (3/31/13)
    # It is sometimes necessary to temporarily comment out the QMAKE_POST_LINK (ln 146) while compiling
    # in Qt Creator b/c it may complain about doc/compile_doc.pl being missing during each build.

    #QMAKE_POST_LINK += perl doc/compile_doc.pl

    # set the final destinations of all install files
    
    man.files = robojournal.7.gz
    man.path = /usr/share/man/man7
    
    documentation.path = /usr/share/doc/robojournal
    documentation.files = doc/robojournal.qch doc/robojournal.qhc
    
    shortcut.path = /usr/share/applications
    shortcut.files = menus/robojournal.desktop

    icon.path = /usr/share/icons
    icon.files = robojournal64.png

    shortcut-deb.path = /usr/share/menu
    shortcut-deb.files = menus/robojournal

    icon-deb.path = /usr/share/pixmaps
    icon-deb.files = menus/robojournal.xpm

    INSTALLS += target shortcut icon shortcut-deb icon-deb documentation man
}

# This one controls Windows instructions for building with MinGW.
win32{
    CONFIG += qt release
    message(Creating 32-bit Windows release build...)
}














