/*
    This file is part of RoboJournal.
    Copyright (c) 2014 by Will Kraft <pwizard@gmail.com>.


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


#include "helpcore.h"
#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include "ui/mainwindow.h"
#include <QDir>
#include <iostream>

Helpcore::Helpcore()
{
}

//#################################################################################
// Show the documentation via Qt Assistant. This used to be part of Mainwindow but
// I split it out in 0.5 so other classes could call Help without redundant code.
// -- Will Kraft (6/21/14).
void Helpcore::Show_Documentation(){

    using namespace std;

    QString assistant, compiled_help_path, collection_path;

    /* Use separate #ifdef blocks to set the paths for Windows and Unix-like operating systems like
     * Linux b/c the file structure is completely different on the two OSes. RoboJournal on Windows
     * requires Qt Assistantto be installed locally (in the same directory) while Linux is able to
     * make use of global system resources.
     *
     * -- Will Kraft, 3/31/13.
     */
#ifdef _WIN32
    // Qt Assistant should be installed in the same folder as robojournal.exe on Win32.
    assistant=QDir::currentPath() + QDir::separator() + "assistant.exe";
    compiled_help_path=QDir::currentPath() + QDir::separator() + "robojournal.qch";
    collection_path=QDir::currentPath() + QDir::separator() + "robojournal.qhc";
#endif

#ifdef unix

    // post-release bugfix for Fedora users (4/26/13)
    // have RoboJournal use the Fedora path to Assistant if applicable.
    QFile fedoracheck("/usr/bin/assistant-qt4");

    if(fedoracheck.exists()){
        assistant="/usr/bin/assistant-qt4";
    }
    else{
        assistant="/usr/bin/assistant";
    }

    compiled_help_path="/usr/share/doc/robojournal/robojournal.qch";
    collection_path="/usr/share/doc/robojournal/robojournal.qhc";
#endif

    QFile assistant_exec(assistant);

    if(assistant_exec.exists()){
        QFile collection_file(collection_path);
        QFile documentation_file(compiled_help_path);

        cout << "OUTPUT: Attempting to find help collection file located at "
             << collection_file.fileName().toStdString() << endl;
        cout << "OUTPUT: Attempting to find compiled documentation file located at "
             << documentation_file.fileName().toStdString() << endl;

        if((collection_file.exists()) && (documentation_file.exists())){

            QProcess *p=new QProcess();
            QStringList args;

#ifdef _WIN32
            args << "-collectionFile" << "robojournal.qhc"
                 << "-enableRemoteControl";
#endif

#ifdef unix
            args << "-collectionFile" << collection_path
                 << "-enableRemoteControl";
#endif
            // Spawn the Assistant process with the OS-specific path-to-binary and args.
            QString exec=assistant + " " + args.join(" ");
            p->start(exec, QIODevice::ReadWrite);

            // Instruct Qt Assistant to completely expand the TOC immediately after launch. This saves
            // the user lots of pointless clicking.
            QByteArray input;
            input.append("expandToc -1\n");
            p->write(input);

            if (!p->waitForStarted()){
                return;
            }
        }
        else{

            // Share this messagebox object between several conditionals... it's just more efficient.
            // In all cases, show appropriate errors if one or more files is missing.
            QMessageBox b;



            if((!collection_file.exists()) && (!documentation_file.exists())){

                QString location;
#ifdef _WIN32
                location=QDir::current().path();
#endif

#ifdef unix
                location="/usr/share/doc/robojournal";
#endif

                b.critical(MainWindow::mw,"RoboJournal","RoboJournal could not locate the collection file"
                           " (robojournal.qhc) or the compiled help file (robojournal.qch). Please"
                           " locate these two files and copy them into <b>" +
                           QDir::toNativeSeparators(location) + "</b>.");

            }
            else{

                if(!collection_file.exists()){
                    b.critical(MainWindow::mw,"RoboJournal","RoboJournal cannot display the documentation because the Collection File "
                               "needed by Qt Assistant is missing. Please copy the Collection File to <b>" +
                               QDir::toNativeSeparators(collection_path) + "</b> and try again.");

                }

                if(!documentation_file.exists()){
                    b.critical(MainWindow::mw,"RoboJournal","RoboJournal cannot display the documentation because the Compiled Help File "
                               "needed by Qt Assistant is missing. Please copy the Compiled Help File to <b>" +
                               QDir::toNativeSeparators(compiled_help_path) + "</b> and try again.");

                }
            }
        }
    }

    // Show error if Qt assistant is not installed
    else{
        QMessageBox c;
        c.critical(MainWindow::mw,"RoboJournal","RoboJournal cannot display the documentation because Qt Assistant"
                   " is not installed correctly.<br><br>Please install (or move) the Qt Assistant executable to <nobr><b>" +
                   QDir::toNativeSeparators(assistant) + "</b></nobr> and try again.");


    }
}
