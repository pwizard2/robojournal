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
#include <QDir>


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

        // initiate database connection. This connection is re-used for all favorites-related
        // connectivity. Use "@favorites" as the global connection name.  --Will Kraft (8/18/13).
        QSqlDatabase base=QSqlDatabase::addDatabase("QSQLITE","@favorites");
        base.setDatabaseName(favorite_db_path);

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

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QSqlQuery update(setFavorite, db);
    update.exec();

    db.close();
}

//###################################################################################################
// Set the favorite status of a database by database name instead of row id number. This was introduced
// because the JournalCreator class needed an easy way for the user to set the new database as a favorite
// without having to do it through preferences (8/18/13).
void FavoriteCore::setFavoritebyName(QString name, bool favorite){

    QString setFavorite;

    switch(favorite){
    case true:
        setFavorite="UPDATE mysql_favorites SET favorite=1 WHERE database=?";
        break;

    case false:
        setFavorite="UPDATE mysql_favorites SET favorite=0 WHERE database=?";
        break;
    }

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QSqlQuery update(setFavorite, db);
    update.bindValue(0,name);
    update.exec();

    db.close();
}

//###################################################################################################
// Create the SQLite favorites database. This only should be used once during firstrun
// if the database does not already exist. New for 0.5. -- Will Kraft, 7/18/13.
void FavoriteCore::Setup_Favorites_Database(){

    // initiate database connection because it should not exist yet if/when this function is called (8/18/13).
    // After this function is complete, the connection we set here is re-used for all favorites-related db
    // transactions for the rest of the session.  Use "@favorites" as the global connection name.

    QSqlDatabase base=QSqlDatabase::addDatabase("QSQLITE","@favorites");
    base.setDatabaseName(favorite_db_path);
    base.open();

    // Update 10/2/13: Add a column to that table that allows each database to set its own range limit.

    QSqlQuery mysql_table_setup("CREATE TABLE mysql_favorites(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "database TEXT, host TEXT, user TEXT, favorite INTEGER, range INTEGER, "
                                "UNIQUE(database) ON CONFLICT IGNORE)",base);

    mysql_table_setup.exec();

    // Use the name "native_favorites" for SQLite table because "sqlite_favorites" is reserved for some reason.
    QSqlQuery sqlite_table_setup("CREATE TABLE native_favorites(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "database TEXT, favorite INTEGER, range INTEGER, UNIQUE(database) ON CONFLICT "
                                 "IGNORE)",base);

    sqlite_table_setup.exec();

    base.close();
}

//###################################################################################################
// Return a QStringList of the favorites for the given username/host. New for 0.5, --Will Kraft 7/18/13.
QList<QStringList> FavoriteCore::getKnownJournals(){

    QList <QStringList> known_journals;

    QSqlDatabase db=QSqlDatabase::database("@favorites");

    db.open();

    QSqlQuery fetch("SELECT id, database, host, user, favorite, range FROM mysql_favorites",db);
    fetch.exec();

    while(fetch.next()){

        QStringList nextitem;

        QVariant v0=fetch.value(0);
        QVariant v1=fetch.value(1);
        QVariant v2=fetch.value(2);
        QVariant v3=fetch.value(3);
        QVariant v4=fetch.value(4);
        QVariant v5=fetch.value(5);

        QString id=v0.toString();
        QString database=v1.toString();
        QString host=v2.toString();
        QString user=v3.toString();
        QString favorite=v4.toString();

        // Hidden independent entry range value for version 0.6. This feature will eventually allow
        // the user to set a different entry range for each journal instead of being limited to a single global
        // value like it is now -- Will Kraft (10/2/13).
        QString range=v5.toString();

        nextitem << id << database << host << user << favorite << range;

        known_journals.append(nextitem);
    }

    db.close();

    return known_journals;
}

//###################################################################################################
// Add an entry to the SQLite favorites database. New for 0.5. --Will Kraft 7/18/13.
// Warning: Input variables should be trustworthy (no SQL injection potential).
void FavoriteCore::Add_to_DB(QString database, QString user, QString host){

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QSqlQuery row("INSERT INTO mysql_favorites(database,host,user,favorite,range) VALUES(?,?,?,?,?)", db);
    row.bindValue(0, database);
    row.bindValue(1, host);
    row.bindValue(2, user);

    // Favorite should be set to false by default (unless the item is the default db). Use int because SQLite
    // doesn't support true booleans.
    int favorite=0;

    // Bugfix (10/2/13): Make sure the default database is marked as a favorite from the very beginning.
    if(database==Buffer::defaultdatabase)
        favorite=1;

    row.bindValue(3, favorite);

    // Use the global entry range as the original value (10/2/13).
    row.bindValue(4,Buffer::entryrange);

    row.exec();
    favorite_db.close();
}

//###################################################################################################
// Remove an entry from the SQLite favorites database. New for 0.5. --Will Kraft 7/18/13.
void FavoriteCore::Remove_from_DB(QString name){

    if(Buffer::backend=="MySQL"){
        QSqlDatabase db=QSqlDatabase::database("@favorites");
        db.open();

        QSqlQuery drop("DELETE FROM mysql_favorites WHERE database=?", db);
        drop.bindValue(0,name);
        drop.exec();

        db.close();
    }

}

//###################################################################################################
// Get the current list of favorites and return it as a QStringList that should be used to populate
// the "Database" combobox on the DBLogin class. New for 0.5. --Will Kraft (7/20/13).
QStringList FavoriteCore::GetFavorites(QString host){

    QStringList favorites;

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QString sql_query="SELECT database FROM mysql_favorites WHERE favorite=1 and host='" + host +"'";

    QSqlQuery fetch(sql_query,db);
    fetch.exec();

    while(fetch.next()){
        QVariant v0=fetch.value(0);
        QString nextitem=v0.toString();

        favorites << nextitem;
    }

    db.close();

    return favorites;
}

//###################################################################################################
// Get the current list of hosts. Most of the time this list only contains one "localhost" entry.
// New for 0.5. --Will Kraft (7/20/13).
QStringList FavoriteCore::GetHosts(){

    QStringList hosts;

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QSqlQuery fetch("SELECT host FROM mysql_favorites",db);
    fetch.exec();

    while(fetch.next()){
        QVariant v0=fetch.value(0);
        QString nextitem=v0.toString();

        // ensure that each item is unique; if nextitem is already on the list, don't add it again.
        if(!hosts.contains(nextitem)){
            hosts << nextitem;
        }
    }

    db.close();

    return hosts;
}

//###################################################################################################
// Do maintenance to see if any databases need to be removed from the favorites list. This is used to clean up
// in case a database got dropped or moved. After all, it wouldn't do to have RoboJournal try to connect to a
// "known" database that doesn't exist anymore. favorites_list refers to the current list of favorites from
// favorites.db while dynamic_list is the list of databases generated from MySQL queries.
// New for 0.5 --Will Kraft (8/20/13).
bool FavoriteCore::Do_Maintenance(QList<QStringList> favorites_list, QStringList dynamic_list){
    using namespace std;

    // first, find out how many journals we still have and add them to the known_journals list.
    QStringList known_journals;

    for(int h=0; h < favorites_list.size(); h++){
        QStringList next=favorites_list.at(h);

        // Bugfix 10/3/13: Restrict the known_journals list to items on the default host (usually localhost/127.0.0.1) only.
        // This keeps the maintenance function from incorrectly deleting items from other hosts b/c they don't appear on
        // the journal list from localhost. I intend to improve this function later to check multiple hosts but this serves
        // as a band-aid solution for now --Will Kraft.
        QString nexthost=next.at(2);
        if(nexthost==Buffer::defaulthost){
            QString next_item=next.at(1);
            known_journals.append(next_item);
        }
    }

    known_journals.sort();
    dynamic_list.sort();

    // Compare the known journals list we just made with the dynamic list from the SQLite database. If the lists are
    // the same, abort. If not, remove the entry that is not on the list from MySQL.

    if(known_journals==dynamic_list){
        cout << "OUTPUT: The list of known MySQL/MariaDB databases is still up-to-date. No further action is required right now." << endl;

        /*
        //Scrap code for showing the contents of the two lists. Used for debugging, comment out in production builds.
        cout << "\n\ndynamic_list items:" << endl;

        for(int i=0; i<dynamic_list.count(); i++){
            cout << dynamic_list.at(i).toStdString() << endl;
        }

        cout << "\n\nknown_journals items:" << endl;

        for(int i=0; i<known_journals.count(); i++){
            cout << known_journals.at(i).toStdString() << endl;
        }
        */

        return false;
    }
    else{
        cout << "OUTPUT: The list of known databases contains items that don't exist anymore. Starting maintenance mode." << endl;

        for(int i=0; i<known_journals.size(); i++){

            QString next=known_journals.at(i);

            // Delete an item that appears in the known_journals list but is NOT currently in the dynamic list from MySQL.
            // It's safe to assume those items no longer exist if the
            if((!dynamic_list.contains(next)) && (Buffer::defaultdatabase != next)){
                Remove_from_DB(next);
                cout << "OUTPUT: Removed " << next.toStdString() << " from the known journals list." << endl;
            }
        }

        return true;
    }
}

//###################################################################################################
// Add an entry to the SQLite favorites database in SQLite mode. New for 0.5. --Will Kraft 9/21/13.
// Warning: Input variables should be trustworthy (no SQL injection potential).
void FavoriteCore::SQLite_Add_to_DB(QString database, bool favorite){

    int fav_value=0;

    if(favorite)
        fav_value=1;

#ifdef unix
    // Shorten the pathname to the user's home directory to unix symbol "~". This helps to prevent
    // horizontal scrolling in the QListItemWidgets that show the sqlite journals.
    database=database.replace(QDir::homePath(),"~");
#endif

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QSqlQuery row("INSERT INTO native_favorites(database,favorite,range) VALUES(?,?,?)", db);
    row.bindValue(0, database);
    row.bindValue(1, fav_value);

    // Use the global entry range as the original value (10/2/13).
    row.bindValue(2,Buffer::entryrange);

    row.exec();
    favorite_db.close();
}

//###################################################################################################
// Return a QStringList of the favorites for the SQLite journals. New for 0.5, --Will Kraft 9/21/13.
QList<QStringList> FavoriteCore::SQLite_getKnownJournals(){

    QList <QStringList> known_journals;

    QSqlDatabase db=QSqlDatabase::database("@favorites");

    db.open();

    QSqlQuery fetch("SELECT id, database, favorite, range FROM native_favorites",db);
    fetch.exec();

    while(fetch.next()){

        QStringList nextitem;

        QVariant v0=fetch.value(0);
        QVariant v1=fetch.value(1);
        QVariant v2=fetch.value(2);
        QVariant v3=fetch.value(3);

        QString id=v0.toString();
        QString database=v1.toString();
        QString favorite=v2.toString();

        // Hidden independent entry range value for version 0.6. This feature will eventually allow
        // the user to set a different entry range for each journal instead of being limited to a single global
        // value like it is now -- Will Kraft (10/2/13).
        QString range=v3.toString();

        nextitem << id << database << favorite << range;

        known_journals.append(nextitem);
    }

    db.close();

    return known_journals;
}

//###################################################################################################
// Set the favorite status of a database by database name instead of row id number. This was introduced
// because the JournalCreator class needed an easy way for the user to set the new database as a favorite
// without having to do it through preferences (8/18/13). Adapted for SQLite favorites (9/21/13).
void FavoriteCore::SQLite_setFavoritebyName(QString name, bool favorite){

    QString setFavorite;

    switch(favorite){
    case true:
        setFavorite="UPDATE native_favorites SET favorite=1 WHERE database=?";
        break;

    case false:
        setFavorite="UPDATE native_favorites SET favorite=0 WHERE database=?";
        break;
    }

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QSqlQuery update(setFavorite, db);
    update.bindValue(0,name);
    update.exec();

    db.close();
}

//###################################################################################################
// Get the current list of favorites and return it as a QStringList that should be used to populate
// the "Database" combobox on the DBLogin class. New for 0.5. --Will Kraft (7/20/13).
QStringList FavoriteCore::GetSQLiteFavorites(){

    QStringList favorites;

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QString sql_query="SELECT database FROM native_favorites WHERE favorite=1";

    QSqlQuery fetch(sql_query,db);
    fetch.exec();

    while(fetch.next()){
        QVariant v0=fetch.value(0);
        QString nextitem=v0.toString();

        favorites << nextitem;
    }

    db.close();

    return favorites;
}

//###################################################################################################
// Scan the contents of the My Journals folder and add all databases if they aren't in the favorite db already.
// This feature prevents the user from having to use the Journal Selector to add the databases again.
// --Will Kraft (10/5/13).
void FavoriteCore::Auto_Populate_SQLite_Favorites(){
    using namespace std;

    if(Buffer::use_my_journals){

        QString homepath=QDir::homePath() + QDir::separator() + "Documents" + QDir::separator() + "My Journals";
        QDir favorite_dir(homepath);

        // Set exclusion filters for the file list (files only, no "." and ".." items, and no symlinks).
        favorite_dir.setFilter(QDir::Files|QDir::NoDotAndDotDot|QDir::NoSymLinks);
        QStringList fileList=favorite_dir.entryList();

        for(int i=0; i < fileList.size(); i++){
            QString next_file=fileList.at(i);

#ifdef unix
            // Shorten the pathname to the user's home directory to unix symbol "~". This helps to prevent
            // horizontal scrolling in the QListItemWidgets that show the sqlite journals.
            homepath=homepath.replace(QDir::homePath(),"~");
#endif
            next_file=homepath + QDir::separator() + next_file;
            next_file=QDir::toNativeSeparators(next_file);

            // The database already has measures to prevent duplicate entries so there is no reason to
            // see if an entry already exists before adding it.
            if(next_file==Buffer::sqlite_default){
                SQLite_Add_to_DB(next_file,1);
            }
            else{
                SQLite_Add_to_DB(next_file,0);

                // Used for debugging, comment out this line in production builds.
                //cout << "Added " << next_file.toStdString() << endl;
            }
        }
    }
}

//###################################################################################################
// Remove specified QString "database" from the list of known SQLite databases. --Will Kraft (10/5/13).
void FavoriteCore::Remove_SQLite_Favorite(QString database){

    QSqlDatabase db=QSqlDatabase::database("@favorites");
    db.open();

    QString sql_query="DELETE FROM native_favorites WHERE database=?";

    QSqlQuery remove(sql_query,db);
    remove.bindValue(0,database);
    remove.exec();

    db.close();
}

//###################################################################################################
// Scan the contents of the favorites db and verify that each file listed there actually exists. If a file
// does not exist anymore, remove it from the database --Will Kraft (10/5/13).
void FavoriteCore::Do_Maintenance_SQLite(){
    using namespace std;

    QStringList favorites=GetSQLiteFavorites();

    for(int i=0; i < favorites.size(); i++){

        QString next_file=favorites.at(i);

#ifdef unix
        // Replace shorthand ~ to home path on Linux systems. This prevents the app from removing journals that
        // still exist because Qt doesn't recognize the home abbreviation symbol.
        next_file=next_file.replace("~",QDir::homePath());
#endif

        QFile current(next_file);

        if(!current.exists()){

#ifdef unix
            // Shorten the pathname to the user's home directory to unix symbol "~". This helps to prevent
            // horizontal scrolling in the QListItemWidgets that show the sqlite journals.
            next_file=next_file.replace(QDir::homePath(),"~");
#endif
            Remove_SQLite_Favorite(next_file);

            cout << "OUTPUT: Removing " << next_file.toStdString() <<
                    " from the list of known journals because that file no longer exists." << endl;
        }
    }
}
