/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
    MADE IN USA

    RoboJournal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RoboJournal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RoboJournal.  If not, see <http://www.gnu.org/licenses/>.
  */


#include <QtGui/QApplication>
#include "ui/mainwindow.h"
#include "ui/editor.h"
#include "core/buffer.h"
#include "core/settingsmanager.h"
#include <iostream>
#include <QtSql/QSqlDatabase>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    using namespace std;

    a.setApplicationName("robojournal");
    a.setApplicationVersion(Buffer::version);

    // Show start-up text in terminal.
    cout << "=============================================="<< endl;
    cout << "\tSTARTING ROBOJOURNAL " << Buffer::version.toStdString() << endl;
    cout << "\n   \"Free Journal Software For Everyone\"" << endl;
    cout << "=============================================="<< endl;

    cout << "STAGE 1: LOADING CONFIGURATION DATA" << endl;
    cout << "=============================================="<< endl;
    cout << "OUTPUT: Running as process ID: "<<  a.applicationPid() << endl;

    // Load the config data into memory. SettingsManager replaces the old ConfigManager class on versions ≥ 0.4
    SettingsManager j;

    j.InstallDictionaries();

    j.LoadConfig();

    // This ifdef block came from Clementine 1.0.1 source.
#ifdef Q_OS_LINUX
  // Force RoboJournal's menu to be shown in the MainWindow and not in
  // the Unity global menubar thing.  See:
  // https://bugs.launchpad.net/unity/+bug/775278
  setenv("QT_X11_NO_NATIVE_MENUBAR", "1", true);
#endif

  // so did this one
#ifndef Q_OS_DARWIN
  // Gnome on Ubuntu has menu icons disabled by default.  I think that's a bad
  // idea, and makes some menus in RoboJournal look confusing. This flag primarily
  // affects the spell check icons in the context menu in the Editor class.
  QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
#else
  QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    // Show Main GUI
    MainWindow w;
    cout << "=============================================="<< endl;
    cout << "Stage 2: SHOWING MAIN WINDOW" <<endl;
    cout << "=============================================="<< endl;
    w.show();



    return a.exec();
}
