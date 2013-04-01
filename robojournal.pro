#-------------------------------------------------
#
# Project created by QtCreator 2011-08-31T19:59:51
#
#-------------------------------------------------

# User-changable values (3/31/13)


# This should be the current version of this codebase. This value is used in the help file
# install path; it should be changed for each version in order to keep the help files for
# different versions separate.
CURRENT_VERSION=0.4.1


# Do *NOT* edit below this line!
######################################################

# Declaration of all Qt modules that should be built into the binary.
QT += core gui sql webkit

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
	ui/journalcreator.cpp \
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
	ui/hunspell/affentry.cxx \
	ui/hunspell/affixmgr.cxx \
	ui/hunspell/csutil.cxx \
	ui/hunspell/dictmgr.cxx \
	ui/hunspell/filemgr.cxx \
	ui/hunspell/hashmgr.cxx \
	ui/hunspell/hunspell.cxx \
	ui/hunspell/hunzip.cxx \
	ui/hunspell/phonet.cxx \
	ui/hunspell/suggestmgr.cxx \
	ui/hunspell/utf_info.cxx \
	ui/SpellTextEdit.cpp \
	ui/highlighter.cpp \
	sql/sqlshield.cpp \
	ui/tagreminder.cpp

HEADERS  += ui/mainwindow.h \
	core/buffer.h \
	ui/editor.h \
	ui/dblogin.h \
	sql/mysqlcore.h \
	sql/psqlcore.h \
	sql/sqlitecore.h \
	ui/aboutrj.h \
	ui/firstrun.h \
	ui/journalcreator.h \
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
	ui/hunspell/affixmgr.hxx \
	ui/hunspell/atypes.hxx \
	ui/hunspell/baseaffix.hxx \
	ui/hunspell/csutil.hxx \
	ui/hunspell/dictmgr.hxx \
	ui/hunspell/filemgr.hxx \
	ui/hunspell/hashmgr.hxx \
	ui/hunspell/htypes.hxx \
	ui/hunspell/hunspell.h \
	ui/hunspell/hunspell.hxx \
	ui/hunspell/hunzip.hxx \
	ui/hunspell/langnum.hxx \
	ui/hunspell/phonet.hxx \
	ui/hunspell/suggestmgr.hxx \
	ui/hunspell/w_char.hxx \
	ui/SpellTextEdit.h \
	ui/highlighter.h \
	sql/sqlshield.h \
	ui/tagreminder.h


FORMS    += ui/mainwindow.ui \
	ui/editor.ui \
	ui/dblogin.ui \
	ui/aboutrj.ui \
	ui/firstrun.ui \
	ui/journalcreator.ui \
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
	ui/tagreminder.ui

# Declaration of images and other to-be-embedded resources.
RESOURCES  = images.qrc

# The RC_File (resource file) sets the program icon for the Win32 executable robojournal.exe.
RC_FILE = icon.rc

# This block contains all Linux/Unix-specific build instructions.
unix {
    CONFIG += qt release

    # The package block sets a different install path for package builds.
    package {
	message(Creating a package-friendly build for Debian and other Linux distros...)
	target.path = /usr/bin
    }
    else{
	message(Creating a regular build...)
	target.path = /usr/local/bin
    }

    # Tweak for development/debugging: (3/31/13)
    # It is sometimes necessary to temporarily comment out the QMAKE_POST_LINK (ln 146) while compiling
    # in Qt Creator b/c it may complain about doc/compile_doc.sh being missing during each build.
    QMAKE_POST_LINK += bash doc/compile_doc.sh

    # set the final destinations of all install files
    doc.path= /usr/share/doc/robojournal-$${CURRENT_VERSION}
    doc.files= doc/robojournal.qhc doc/robojournal.qch

    shortcut.path = /usr/share/applications
    shortcut.files = menus/robojournal.desktop

    icon.path = /usr/share/icons
    icon.files = robojournal64.png

    shortcut-deb.path = /usr/share/menu
    shortcut-deb.files = menus/robojournal

    icon-deb.path = /usr/share/pixmaps
    icon-deb.files = menus/robojournal.xpm

    INSTALLS += target shortcut icon shortcut-deb icon-deb doc

}

# This one controls Windows instructions for building with MinGW.
win32{
    CONFIG += qt release
    message(Creating 32-bit Windows release build...)
}
