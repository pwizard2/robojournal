#-------------------------------------------------
#
# Project created by QtCreator 2011-08-31T19:59:51
#
#-------------------------------------------------

QT       += core gui

TARGET = robojournal
TEMPLATE = app


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

RESOURCES  = images.qrc

QT += sql gui webkit

RC_FILE = icon.rc

unix {
        CONFIG += qt release

        package {
            message(Creating a makefile with the correct install path for packaging purposes...)
            target.path = /usr/bin
        }
        else{
            message(Creating a regular build...)
            target.path = /usr/local/bin
        }
        
        QMAKE_POST_LINK += bash doc/compile_doc.sh
        
	doc.path= /usr/share/doc/robojournal-0.4.1
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


win32{
        CONFIG += qt release
        message(Creating 32-bit Windows release build...)
}
