/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
    

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


#include "sql/sqlitecore.h"
#include "core/buffer.h"
#include <iostream>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QMessageBox>
#include "ui/mainwindow.h"

SQLiteCore::SQLiteCore(){}

QSqlDatabase SQLiteCore::db;

//################################################################################################
// Check to see if the SQLite database exists. If it does, create a new connection to it. --Will Kraft (2/16/14).
bool SQLiteCore::Connect(QString dbname){
    using namespace std;
    cout << "OUTPUT: Attempting SQLite (failsafe) database connection on \"" << Buffer::host.toStdString()
         << "\" as user \"" << Buffer::username.toStdString() << "\"...";

    QFile database(dbname);

    if(!database.exists()){
        QMessageBox m;
        m.critical(MainWindow::mw,"RoboJournal","The database file <b>" + dbname + "</b> does not exist.");
        return false;
    }

    QSqlDatabase db2=QSqlDatabase::addDatabase("QSQLITE","@lite");
    db2.setDatabaseName(dbname);

    bool success=db2.open();


    if(success){
        cout << "SUCCESS!"<< endl;
    }
    else{
        cout << "FAILED!" << endl;
    }



    return success;
}


bool SQLiteCore::Disconnect(){
    bool success=false;
    return success;
}


bool SQLiteCore::AddEntry(){
    bool success=false;
    return success;

}

bool SQLiteCore::RemoveEntry(){
    bool success=false;
    return success;

}

//################################################################################################
// Create a new database file. Unlike MySQL, simply attempting to open a null file causes it to be created.
bool SQLiteCore::CreateDB(QString dbname){
    using namespace std;
    cout << "OUTPUT: Attempting to create database " << dbname.toStdString() << endl;

    QSqlDatabase db2= QSqlDatabase::addDatabase("QSQLITE","@lite");
    db2.setDatabaseName(dbname);
    db2.open();

    QSqlQuery create_entries("CREATE TABLE entries(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "title TEXT, month TEXT, day TEXT, year TEXT, tags TEXT, "
                             "body TEXT, time TEXT)", db2);

    create_entries.exec();

    db2.close();

    // if the process worked, the file should exist.
    QFile database(dbname);
    return database.exists();
}


