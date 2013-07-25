/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
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

    Class description/purpose (7/18/13) --Will Kraft:
    The FavoriteCore class is responsible for managing the list of
    favorite databases. If the user has several databases that are used
    on a regular basis, the favorites system allows easy switching.
    In the past, it was necessary to redefine the default or enter
    the database name manually at the start of each connection. This class
    should be included in all other classes that interact with the
    favorites system.
*/

#include "favoritecore.h"
#include <QtSql/QSqlDatabase>
#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlResult>
#include <iostream>
#include "core/buffer.h"
#include <QList>

//###################################################################################################
FavoriteCore::FavoriteCore()
{
    // This is the absolute path to the favorite.db database.
    favorite_db_path=QDir::homePath() + QDir::separator() + ".robojournal" +
            QDir::separator() + "favorites.db";

    favorite_db_path=QDir::toNativeSeparators(favorite_db_path);
}

//###################################################################################################
// Run this code during Stage 1 when RoboJournal starts up to see if the favorites database exists.
// New for 0.5. --Will Kraft 7/18/13.
void FavoriteCore::init(){
    using namespace std;

    cout << "OUTPUT: Checking for favorites database.......................";

    QFile db(favorite_db_path);

    if(db.exists()){
        cout << "[OK]" << endl;

    }
    else{
        cout << "[FAILED]" << endl;
        cout << "OUTPUT: Creating new favorites database.......................";

        // Re-create database if it does not exist at RoboJournal startup.
        Setup_Favorites_Database();

        cout << "[OK]" << endl;
    }
}

//###################################################################################################
// Change the Favorite status of QString "database" by assigning bool Favorite.
// New for 0.5. -- Will Kraft, 7/19/13.
void FavoriteCore::setFavorite(QString id, bool favorite){

    QString setFavorite;

    switch(favorite){
    case true:
        setFavorite="UPDATE mysql_favorites SET favorite=1 WHERE id=" + id;
        break;

    case false:
        setFavorite="UPDATE mysql_favorites SET favorite=0 WHERE id=" + id;
        break;
    }

    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);

    favorite_db.open();

    QSqlQuery update(setFavorite);
    update.exec();

    favorite_db.close();
}

//###################################################################################################
// Create the SQLite favorites database. This only should be used once during firstrun
// if the database does not already exist. New for 0.5. -- Will Kraft, 7/18/13.
void FavoriteCore::Setup_Favorites_Database(){

    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);

    favorite_db.open();

    QSqlQuery mysql_table_setup("CREATE TABLE mysql_favorites(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "database TEXT, host TEXT, user TEXT, favorite INTEGER, UNIQUE(database) "
                                "ON CONFLICT IGNORE)");

    mysql_table_setup.exec();

    // Use the name "native_favorites" for SQLite table because "sqlite_favorites" is reserved for some reason.
    QSqlQuery sqlite_table_setup("CREATE TABLE native_favorites(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "database TEXT, favorite INTEGER, UNIQUE(database) ON CONFLICT IGNORE)");

    sqlite_table_setup.exec();

    favorite_db.close();
}

//###################################################################################################
// Return a QStringList of the favorites for the given username/host. New for 0.5, --Will Kraft 7/18/13.
QList<QStringList> FavoriteCore::getKnownJournals(){

    QList <QStringList> known_journals;

    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);

    favorite_db.open();

    QSqlQuery fetch("SELECT id, database, host, user, favorite FROM mysql_favorites");
    fetch.exec();

    while(fetch.next()){

        QStringList nextitem;

        QVariant v0=fetch.value(0);
        QVariant v1=fetch.value(1);
        QVariant v2=fetch.value(2);
        QVariant v3=fetch.value(3);
        QVariant v4=fetch.value(4);

        QString id=v0.toString();
        QString database=v1.toString();
        QString host=v2.toString();
        QString user=v3.toString();
        QString favorite=v4.toString();

        nextitem << id << database << host << user << favorite;

        known_journals.append(nextitem);
    }

    favorite_db.close();

    return known_journals;
}

//###################################################################################################
// Add an entry to the SQLite favorites database. New for 0.5. --Will Kraft 7/18/13.
// Warning: Input variables should be trustworthy (no SQL injection potential).
void FavoriteCore::Add_to_DB(QString database, QString user, QString host){

    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);
    favorite_db.open();

    QSqlQuery row("INSERT INTO mysql_favorites(database,host,user,favorite) VALUES(?,?,?,?)");
    row.bindValue(0, database);
    row.bindValue(1, host);
    row.bindValue(2, user);

    // Favorite should be set to false by default. Use int because SQLite doesn't support true booleans.
    row.bindValue(3, 0);


    row.exec();
    favorite_db.close();
}

//###################################################################################################
// Remove an entry from the SQLite favorites database. New for 0.5. --Will Kraft 7/18/13.
void FavoriteCore::Remove_from_DB(QString id){



    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);

}

//###################################################################################################
// Get the current list of favorites and return it as a QStringList that should be used to populate
// the "Database" combobox on the DBLogin class. New for 0.5. --Will Kraft (7/20/13).
QStringList FavoriteCore::GetFavorites(QString host){

    QStringList favorites;

    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);
    favorite_db.open();

    QSqlQuery fetch("SELECT database FROM mysql_favorites WHERE favorite=1 and host='" + host +"'");
    fetch.exec();

    while(fetch.next()){
        QVariant v0=fetch.value(0);
        QString nextitem=v0.toString();

        favorites << nextitem;
    }

    favorite_db.close();

    return favorites;
}

//###################################################################################################
// Get the current list of hosts. Most of the time this list only contains one "localhost" entry.
// New for 0.5. --Will Kraft (7/20/13).
QStringList FavoriteCore::GetHosts(){

    QStringList hosts;

    favorite_db=QSqlDatabase::addDatabase("QSQLITE");
    favorite_db.setDatabaseName(favorite_db_path);
    favorite_db.open();

    QSqlQuery fetch("SELECT host FROM mysql_favorites");
    fetch.exec();

    while(fetch.next()){
        QVariant v0=fetch.value(0);
        QString nextitem=v0.toString();

        // ensure that each item is unique; if nextitem is already on the list, don't add it again.
        if(!hosts.contains(nextitem)){
            hosts << nextitem;
        }
    }

    favorite_db.close();

    return hosts;
}

//###################################################################################################
// Do maintenance to see if any databases need to be removed from the favorites list. This is used to clean up
// in case a database got dropped or moved. After all ,it wouldn't do to have RoboJournal try to connect to a
// "known" database that doesn't exist anymore. favorites_list refers to the current list of favorites from
// favorites.db while dynamic_list is the list of databases generated from MySQL queries.
// New for 0.5 --Will Kraft (7/24/13).
void FavoriteCore::Do_Maintenance(QList<QStringList> favorites_list, QStringList dynamic_list){

    for(int i=0; i < dynamic_list.size(); i++){

    }
}
