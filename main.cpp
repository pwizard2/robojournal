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
#include "core/favoritecore.h"


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

    // Check for arguments. New for 0.5, 8/23/13.
    QStringList args=a.arguments();

    QRegExp date("--date-override=\\d{0,2}-\\d{0,2}-\\d{0,4}");

    for(int i=0; i<args.length(); i++){
        QString next_arg=args.at(i);

        // Check to see if Robojournal should override the date for this session. FYI: This feature can be dangerous (8/23/13).
        if(date.exactMatch(next_arg)){



            QString date=next_arg.section("",17,next_arg.length());

            QStringList d=date.split("-");
            int month=d.at(0).toInt();
            int day=d.at(1).toInt();
            int year=d.at(2).toInt();
            QTime time=QTime::currentTime();
            QDate custom;
            custom.setYMD(year,month,day);

            if(custom.isValid()){
                Buffer::use_date_override=true;
                Buffer::override_date.setDate(custom);
                Buffer::override_date.setTime(time);

                cout << "OUTPUT: Date override command accepted. Setting date to "
                     << Buffer::override_date.toString("MM-dd-yyyy.").toStdString() << endl;
            }
            else{
                cout << "OUTPUT: Date override command rejected due to invalid date value." << endl;
                Buffer::use_date_override=false;
            }
        }
    }

    // Load the config data into memory. SettingsManager replaces the old ConfigManager class on versions ≥ 0.4
    SettingsManager j;

    j.InstallDictionaries();

    // Load Favorites Core. New for 0.5 (7/18/13).
    FavoriteCore f;
    f.init();

    j.LoadConfig(true);



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
