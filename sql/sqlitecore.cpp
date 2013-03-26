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


#include "sql/sqlitecore.h"
#include "core/buffer.h"
#include <iostream>
#include <QtSql/QSqlDatabase>


SQLiteCore::SQLiteCore(){}

QSqlDatabase SQLiteCore::db;

bool SQLiteCore::Connect(){
    using namespace std;
    cout << "OUTPUT: Attempting SQLite (failsafe) database connection on \"" << Buffer::host.toStdString()
         << "\" as user \"" << Buffer::username.toStdString() << "\"...";

    db=QSqlDatabase::addDatabase("QSQLITE");

    QString host=Buffer::host;
    QString database=Buffer::database_name;
    QString user=Buffer::username;
    QString password=Buffer::password;
    //int port=Buffer::databaseport.toInt();

    db.setHostName(host);
    //db.setPort(port);
    //db.setUserName(user);
     db.setDatabaseName(database);
    //db.setPassword(password);

    bool success=db.open();


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


bool SQLiteCore::CreateDB(){
    bool success=false;
    return success;

}


