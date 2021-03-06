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

    UPDATE 8/18/13: Use "@mysql" as the global connection name for all regular
    database transactions. Functions that require root access should use something
    different (e.g. @create, @MASTER , etc.) to avoid breaking the current journal
    connection.

  */


#include "sql/mysqlcore.h"
#include "core/buffer.h"
#include <iostream>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include "ui/editor.h"
#include <QSqlResult>
#include <QStringList>
#include <QDebug>
#include <QTime>
#include <QRegExp>
#include "sql/sqlshield.h"
#include <QApplication>
#include "ui/mainwindow.h"
#include <QtNetwork/QNetworkInterface>
#include <QSqlError>


QSqlDatabase MySQLCore::db;
int MySQLCore::dialogX;
int MySQLCore::dialogY;
int MySQLCore::ID;
QString MySQLCore::error_code; // not really used

QString MySQLCore::recordnum;

//################################################################################################

MySQLCore::MySQLCore()
{
}

//################################################################################################
// New feature for 0.5, 6/23/13 -- Will Kraft.
// Use the root account to grant permission to use "database" to user "username". Create username account if necessary.
bool MySQLCore::GrantPermissions(bool create_account, QString database, QString user_host, QString username, QString port,
                                 QString root_host, QString user_password, QString root_password){

    bool success=true; //success should be true unless something goes wrong and changes it to false
    int db_port=port.toInt();

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL","@grant");
    db2.setHostName(root_host);
    db2.setPort(db_port);
    db2.setUserName("root");
    db2.setPassword(root_password);
    db2.open();

    // Bugfix 8/21/12: Save old login data in case login fails. That way other connections won't get broken after
    // exiting JournalSelector
    old_username2=db2.userName();
    old_password2=db2.password();

    if(create_account){
        QSqlQuery q("CREATE USER \'" + username + "\'@\'" + user_host +
                    "\' IDENTIFIED BY \'" + user_password + "\'",db2);
        q.exec();
    }

    QSqlQuery q2("GRANT INSERT,DELETE,UPDATE,SELECT,RELOAD ON " + database + ".entries TO \'" +
                 username + "\'@\'" + user_host + "\'",db2);
    success=q2.exec();

    // Flush privileges on host after grant so the new changes are available immediately (Will Kraft 4/6/14).
    QSqlQuery q3("FLUSH PRIVILEGES",db2);
    q3.exec();

    db2.close();

    // if login fails, restore the old values before closing the database. That way, the connections in the rest of
    // the app won't get broken.
    if(!success){
        db2.setUserName(old_username2);
        db2.setPassword(old_password2);
        // clean up
        old_password2.clear();
        old_username2.clear();
    }

    // Bugfix (8/19/13) Change db2's association so we can remove it without creating warnings.
    db2 = QSqlDatabase();
    db2.removeDatabase("@grant");

    return success;
}

//################################################################################################
// New tag reminder function for 0.4.1 (added 2/25/13). This code does a lookup for rows where tags are null
// (i.e. this entry has not been tagged yet) and returns the results as a qlist full of qstringlists.

QList<QStringList> MySQLCore::NullSearch(){

    QList<QStringList> values;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QSqlQuery q("SELECT id, title, day, month, year FROM entries WHERE tags=\"NULL\" ORDER BY id DESC",db2);

    q.exec();



    while(q.next()){
        QStringList entry;
        QVariant v0=q.value(0); //id;
        QVariant v1=q.value(1); //title
        QVariant v2=q.value(2); //day
        QVariant v3=q.value(3); //month
        QVariant v4=q.value(4); //year

        QString id=v0.toString();
        QString title=v1.toString();

        // process date according to settings
        QString day=v2.toString();
        QString month=v3.toString();
        QString year=v4.toString();

        QString datestamp;

        switch(Buffer::date_format){
        case 0: // European
            datestamp= day + "-" + month + "-" + year;
            break;

        case 1: // USA Style
            datestamp= month + "-" + day + "-" + year;
            break;

        case 2: // ISO
            datestamp= year + "-" + month + "-" + day;
            break;
        }

        entry << id << title << datestamp;

        //append the row to the main list
        values.append(entry);
    }

    db2.close();

    return values;
}


//################################################################################################

/*
 * Two new UNIFIED entry retrieval functions for 0.5 that get all necessary data at once (returned as StringList) to reduce
 * database overhead. The reason why these queries were originally done separately is that each new feature (timestamp,
 * tagging, etc.) was added separately over time/different versions and those standalone functions were always meant to be
 * temporary kludges anyway. Since MySQL functionality is pretty much finished by this point (11/28/12) it's time to be much more efficient.
 *
 * As of the 0.5 release, the old code blocks that handled these same features ( RetrieveEntryFull(), GetTimespamp(), GetTags(), GetTitle(), etc.)
 * are DEPRECATED and should not be called anymore. However, leave the commented-out legacy code in the source for reference purposes.
 *
 * UPDATE 11/28/12: DO **NOT** IMPLEMENT THIS UNTIL 0.5 DEV CYCLE! The entry retrieval system is getting a full overhaul in that release
 * anyway so it's better to save this until then. The old functions are still good enough for 0.4's requirements.
 *
 */

QStringList MySQLCore::Get_Entry_New(QString id){

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();
    //db.open();

    QSqlQuery q("SELECT title, month, day, year, time, tags, body FROM entries WHERE id=?",db2);
    q.bindValue(0,id);
    q.exec();

    QStringList entry;

    while(q.next()){
        QVariant v0=q.value(0); //title;
        QVariant v1=q.value(1); //month
        QVariant v2=q.value(2); //day
        QVariant v3=q.value(3); //year
        QVariant v4=q.value(4); //time
        QVariant v5=q.value(5); //tags
        QVariant v6=q.value(6); // body

        entry.append(v0.toString());

        // process date according to settings
        QString month=v1.toString();
        QString day=v2.toString();
        QString year=v3.toString();

        QString datestamp;

        switch(Buffer::date_format){
        case 0: // European
            datestamp= day + "-" + month + "-" + year;
            break;

        case 1: // USA Style
            datestamp= month + "-" + day + "-" + year;
            break;

        case 2: // ISO
            datestamp= year + "-" + month + "-" + day;
            break;
        }

        entry.append(datestamp);

        // process timestamp

        QString timestamp=v4.toString();
        // Process null times
        if(timestamp.isEmpty()){
            timestamp="[unknown time]";
        }
        else{
            // Raw time format is 24-hour style. If user wants AM/PM, we must convert it
            if(!Buffer::use_24_hour){
                QTime time=QTime::fromString(timestamp,"HH:mm");
                timestamp=time.toString("h:mm ap");

            }
        }

        entry.append(timestamp);

        // process tags and body

        entry.append(v5.toString()); // tags
        entry.append(v6.toString()); // body
    }

    db2.close();

    return entry;
}


//QStringList MySQLCore::Get_Latest_New(QString id){

//}

//################################################################################################
// Search database, new for 0.4
QList<QStringList> MySQLCore::SearchDatabase(QString searchterm, int index, QString tag, bool wholewords){

    QList<QStringList> results;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();
    //db.open();
    QString query;


    if(wholewords){
        switch(index){

        // Search from tag list
        case 0:
            query="SELECT id,title,month,day,year,body FROM entries WHERE tags RLIKE BINARY '[[:<:]]" + tag + "[[:>:]]' ORDER BY id DESC";
            break;

            // Search in title
        case 1:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE title RLIKE BINARY '[[:<:]]" + searchterm + "[[:>:]]' ORDER BY id DESC";
            break;

            // Search in body text
        case 2:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE body RLIKE BINARY '[[:<:]]" + searchterm + "[[:>:]]' ORDER BY id DESC";
            break;

            // Search in title and body text
        case 3:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE title RLIKE BINARY '[[:<:]]" + searchterm +
                    "[[:>:]]' OR body RLIKE BINARY '[[:<:]]" + searchterm + "[[:>:]]' ORDER BY id DESC";
            break;
        }
    }
    else{
        switch(index){

        // Search from tag list
        case 0:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE tags RLIKE BINARY '" + tag + "' ORDER BY id DESC";
            break;

            // Search in title
        case 1:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE title RLIKE BINARY '" + searchterm + "' ORDER BY id DESC";
            break;

            // Search in body text
        case 2:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE body RLIKE BINARY '" + searchterm + "' ORDER BY id DESC";
            break;

            // Search in title and body text
        case 3:
            query="SELECT id,title,month,day,year,body  FROM entries WHERE title RLIKE BINARY '" + searchterm +
                    "' OR body RLIKE BINARY '" + searchterm + "' ORDER BY id DESC";
            break;
        }
    }

    // Do sql injection protection on this query before running it. Most of the others are safe b/c
    // of bound parameters but this one is not. Break_injections() is not failsafe but better than running raw
    // input.
    SQLShield s;
    QString sanitized=s.Break_Injections(query);
    QSqlQuery q(sanitized,db2);
    q.exec();



    QString timestamp;

    // create a stringlist from query variant data for each entry and append it to the main results QList
    while(q.next()){

        QStringList entry;

        QVariant v0=q.value(0); // id
        QVariant v1=q.value(1); // title

        QString title=v1.toString();

        QVariant v2=q.value(2);
        QString month=v2.toString();

        QVariant v3=q.value(3);
        QString day=v3.toString();

        QVariant v4=q.value(4);
        QString year=v4.toString();

        // insert leading zeros for month and day values. QDate gets broken if we don't do this.
        if(day.toInt() < 10){
            day="0" + day;
        }

        if(month.toInt() < 10){
            month="0" + month;
        }


        switch(Buffer::date_format){
        case 0: // European

            timestamp= day + "/" + month + "/" + year;
            break;

        case 1: // USA Style
            timestamp= month + "/" + day + "/" + year;
            break;
        case 2: // ISO
            timestamp= year + "/" + month + "/" + day;

            break;
        }


        // get number of occurences of term. This is why the query needs to get the title
        // and body even though we don't actually do anything with the body here.
        QVariant v5=q.value(5);
        QString body=v5.toString();

        QString occurences;

        int num1;
        int num2;

        // whole words only in this block, so use a Regexp to ensure that.
        if(wholewords){

            QRegExp reg("(\\b)(" + searchterm + ")(\\b)|(.,:;'\"\')");
            reg.setCaseSensitivity(Qt::CaseSensitive);


            switch(index){
            case 0: // tag search mode so there are no matches in text
                occurences="N/A";
                break;

            case 1: //title only
                num1=title.count(reg);
                occurences=QString::number(num1);
                break;

            case 2: // body only
                num1=body.count(reg);
                occurences=QString::number(num1);
                break;

            case 3: // title + body
                num1=title.count(reg);
                num2=body.count(reg);
                occurences=QString::number(num1+num2);
                break;
            }
        }

        // partial matches are ok in this block
        else{
            switch(index){
            case 0: // tag search mode so there are no matches in text
                occurences="N/A";
                break;

            case 1: //title only
                num1=title.count(searchterm,Qt::CaseSensitive);
                occurences=QString::number(num1);
                break;

            case 2: // body only
                num1=body.count(searchterm,Qt::CaseSensitive);
                occurences=QString::number(num1);
                break;

            case 3: // title + body
                num1=title.count(searchterm,Qt::CaseSensitive);
                num2=body.count(searchterm,Qt::CaseSensitive);
                occurences=QString::number(num1+num2);
                break;
            }
        }



        entry << v0.toString() << title << timestamp << occurences;

        results.append(entry);
    }

    db2.close();

    return results;
}

//################################################################################################
// This is for the "export all entries" function. This loads each entry into a QStringList and then
// adds the stringlist to a QList. (8/8/12)
QList<QStringList> MySQLCore::DumpDatabase(bool asc){

    QList<QStringList> journal;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();
    //db.open();
    QString query;

    // sort ascending
    if(asc){
        query="SELECT title,month,day,year,tags,body,time FROM entries ORDER BY id ASC";
    }
    //sort descending
    else{
        query="SELECT title,month,day,year,tags,body,time FROM entries ORDER BY id DESC";
    }

    QSqlQuery q(query,db2);
    q.exec();

    while(q.next()){
        QVariant v0=q.value(0); // title value
        QVariant v1=q.value(1); // month value
        QVariant v2=q.value(2); // day value
        QVariant v3=q.value(3); // year value
        QVariant v4=q.value(4); // tags value
        QVariant v5=q.value(5); // body value
        QVariant v6=q.value(6); // time value

        QString title=v0.toString();
        QString month=v1.toString();
        QString day=v2.toString();
        QString year=v3.toString();
        QString tags=v4.toString();
        QString body=v5.toString();
        QString time=v6.toString();

        QStringList entry;
        entry << title << month << day << year << tags << body << time;

        // add this entry to QList
        journal.append(entry);

        // wipe entry clean for next row
        entry.clear();
    }

    db2.close();

    return journal;
}

//################################################################################################
// Get list of databases for JournalSelector class
QStringList MySQLCore::GetDatabaseList(QString hostname, QString port, QString username, QString password, bool silentMode){
    using namespace std;

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL","@MASTER");

    int socket=port.toInt();

    // use information_schema database if Buffer::database_name is null.
    // that way, there will not be a segfault due to a a null variable.
    // we don't want to do anything with that database, just link to it.
    if(Buffer::database_name.isEmpty()){
        Buffer::database_name="information_schema";
    }


    db2.setHostName(hostname);
    db2.setPort(socket);
    db2.setUserName(username);
    db2.setPassword(password);
    db2.setDatabaseName(Buffer::database_name);

    QStringList journals;

    // Bugfix 8/8/12: Save old login data in case login fails. That way other connections won't get broken after
    // exiting JournalSelector
    old_username=db2.userName();
    old_password=db2.password();

    bool success=db2.open();


    if(success){
        QSqlQuery q("SHOW DATABASES",db2);

        q.exec();

        QString next;

        while(q.next()){
            QVariant result=q.value(0);
            next=result.toString();

            if((next!="information_schema") && (next!="performance_schema")){
                journals.append(next);
            }


            //cout << next.toStdString() << endl;
        }

        if(journals.size()==0){
            QMessageBox b;
            b.information(MainWindow::mw,"RoboJournal","RoboJournal found no databases belonging to user \""
                          + username + "@" + hostname + "\". You should use the <b>Assign Permissions</b> tool "
                          "to give this account access to one or more databases on the host.");
        }
    }
    else{

        // If there's no MySQL, display an error
        if(!db2.isDriverAvailable("QMYSQL")){
            success=false;
            cout << "OUTPUT: MySQL support was not found! MySQL functionality disabled." << endl;
            QMessageBox a;
            a.critical(MainWindow::mw,"RoboJournal","RoboJournal could not find or use the Qt MySQL driver. "
                       "This problem occurs if the Qt environment used to compile RoboJournal was built without MySQL support.");
        }

        if((db2.isOpenError()) && (!silentMode)){
            QMessageBox m;
            QString reason;

            if((hostname=="localhost") || (hostname=="127.0.0.1")){
                reason="Make sure you entered the correct username/password and try again.";
            }
            else{
                reason="Are you allowed to access <b>" + hostname +
                        "</b> from this computer? If so, make sure you entered the correct username/password and try again.";
            }

            m.critical(MainWindow::mw,"RoboJournal","RoboJournal could not connect to  <b>" +
                       hostname + "</b>.<br><br>" + reason );
        }
    }

    // if login fails, restore the old values before closing the database. That way, the connections in the rest of
    // the app won't get broken.
    if(!success){
        db2.setUserName(old_username);
        db2.setPassword(old_password);
        // clean up
        old_password.clear();
        old_username.clear();
    }

    db2.close();

    // Bugfix (8/19/13) Change db2's association so we can remove it without creating warnings.
    db2 = QSqlDatabase();
    db2.removeDatabase("@MASTER");

    return journals;
}

//################################################################################################
// upgrade journal to RoboJournal >0.1 compatible version by adding [time varchar(5)] column
bool MySQLCore::UpgradeJournal(QString root_pass){

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL","@MASTER");
    db2.setHostName(Buffer::host);
    db2.setDatabaseName(Buffer::database_name);
    db2.setPort(Buffer::databaseport);
    db2.setUserName("root");
    db2.setPassword(root_pass);
    db2.open();

    QSqlQuery q("ALTER TABLE entries ADD time VARCHAR(5)",db2);
    bool success=q.exec();

    db2.close();

    // Bugfix (8/19/13) Change db2's association so we can remove it without creating warnings.
    db2 = QSqlDatabase();
    db2.removeDatabase("@MASTER");

    return success;
}


//################################################################################################
// make sure the current database is "sane" (has the right structure) before attempting to use it.

bool MySQLCore::SanityCheck(){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="DESCRIBE entries";
    QSqlQuery q(query,db2);
    q.exec();
    bool is_sane=false;

    QString pattern="idtitlemonthdayyeartagsbodytime";
    QList<QString> match;
    QString assembler;

    QString output;
    while(q.next()){

        QVariant v=q.value(0);
        output=v.toString();
        //qDebug() << output;
        int pos=0;
        match.append(output);
        pos++;


    }

    for(int m=0; m<match.length(); m++){
        assembler.append(match[m]);

    }

    // Only allow the database to load if the table structure matches the
    // predefined pattern. Other databases are invalid and very bad things will happen if we
    // let RJ use them.
    if(assembler==pattern){
        is_sane=true;
        cout << "OUTPUT: Database " << Buffer::database_name.toStdString() <<
                " passed database sanity check, proceeding with load..." << endl;
    }
    else{

        // triggered if user loads an old journal.
        if(assembler=="idtitlemonthdayyeartagsbody"){
            Buffer::needs_upgrade=true;
        }

    }

    db2.close();
    return is_sane;
}

//################################################################################################
// Connect to the database
bool MySQLCore::Connect(){

    using namespace std;

    cout << "OUTPUT: Attempting MySQL database connection on \"" << Buffer::host.toStdString()
         << "\" as user \"" << Buffer::username.toStdString() << "\"...";

    QString host=Buffer::host;
    QString database=Buffer::database_name;
    QString user=Buffer::username;
    QString password=Buffer::password;
    int port=Buffer::databaseport;

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL","@mysql");
    db2.setHostName(host);
    db2.setPort(port);
    db2.setUserName(user);
    db2.setDatabaseName(database);
    db2.setPassword(password);

    if(Buffer::SSL){
        db2.setConnectOptions("CLIENT_SSL=1");
    }


    bool success=db2.open();

    // The idea here just to see if we can create a connection at all.
    // if success returns true, the login form class DBLogin proceeds and the rest of the
    // program is unlocked. Other parts of the program can then call MysqlCore directly as needed.
    if(success){
        cout << "SUCCESS!"<< endl;
        db2.close();
        return true;
    }
    else{
        cout << "FAILED!" << endl;

        // Bugfix (8/25/13): Destroy the old connection and allow the user to reconnect.
        db2.close();



        if(db2.isOpenError()){
            QMessageBox m;

            QString reason;

            if((Buffer::host=="localhost") || (Buffer::host=="127.0.0.1")){
                reason="Make sure you entered the correct username/password and try again.";
            }
            else{
                reason="Are you allowed to access <b>" + Buffer::database_name +
                        "</b> from this computer? If so, make sure you entered the correct username/password and try again.";
            }

            m.critical(MainWindow::mw,"RoboJournal","RoboJournal could not connect to  <b>" +
                       Buffer::database_name + "</b>@<b>" +
                       Buffer::host + "</b>.<br><br>" + reason );
        }

        // check to make sure the MYSQL driver is installed, return error if false
        // If you're using a static build of QT you're probably never going to see this error
        if(!db2.isDriverAvailable("QMYSQL")){
            QMessageBox j;
            j.critical(MainWindow::mw,"RoboJournal","RoboJournal could not find the MySQL driver. This problem likely occurred"
                       " because the Qt libraries on this computer were compiled incorrectly or are incomplete. RoboJournal"
                       " cannot use MySQL databases until this issue is resolved.");
        }

        QSqlDatabase db2;
        db2.removeDatabase("@mysql");

        return false;
    }
}

//################################################################################################
// I don't think this is even being used anymore
void MySQLCore::Disconnect(){
    using namespace std;
    cout << "OUTPUT: Attempting to close MySQL connection...";

    QSqlDatabase db2;
    db2.removeDatabase("@mysql");
    db2.close();

    cout << "Done!" << endl;


}
//################################################################################################
// Update a row with new data. This is invoked from the Editor class if editmode==true.
bool MySQLCore::Update(QString title, int month, int day, int year,  QString body, QString id, QString tags){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QSqlQuery update("UPDATE entries set title=?, month=?, day=?, year=?, tags=?, body=? WHERE id=" + id,db2);
    update.bindValue(0, title);
    update.bindValue(1, month);
    update.bindValue(2, day);
    update.bindValue(3, year);
    update.bindValue(4, tags); // Add tags parameter for 0.5, 6/29/13.
    update.bindValue(5, body);


    bool success=update.exec();
    db2.close();

    return success;
}
//################################################################################################
bool MySQLCore::UpdateTags(QString tag_data, QString id){

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QSqlQuery tag("UPDATE entries SET tags=? WHERE id=" + id,db2);
    tag.bindValue(0,tag_data);
    bool success=tag.exec();

    db2.close();
    return success;
}


//################################################################################################
bool MySQLCore::AddEntry(){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString title=Editor::title;
    int month=Editor::month;
    int day=Editor::day;
    int year=Editor::year;
    QString tags=Editor::tags;
    QString body=Editor::body;


    bool success;
    // if the user opts to record time, write extra time field
    if(Buffer::keep_time){
        QString insert="INSERT INTO entries(title, month, day, year, tags, body, time) VALUES(?,?,?,?,?,?,?)";
        QSqlQuery addentry(insert,db2);
        addentry.bindValue(0, title);
        addentry.bindValue(1, month);
        addentry.bindValue(2, day);
        addentry.bindValue(3, year);
        addentry.bindValue(4, tags);
        addentry.bindValue(5, body);

        // get current system time in 24 hour style
        QTime t=QTime::currentTime();
        QString timestamp=t.toString("HH:mm");
        addentry.bindValue(6, timestamp);
        //cout << "OUTPUT: Current timestamp: " << timestamp.toStdString() << endl;
        success=addentry.exec();
    }
    else{
        QString insert="INSERT INTO entries(title, month, day, year, tags, body) VALUES(?,?,?,?,?,?)";
        QSqlQuery addentry(insert,db2);
        addentry.bindValue(0, title);
        addentry.bindValue(1, month);
        addentry.bindValue(2, day);
        addentry.bindValue(3, year);
        addentry.bindValue(4, tags);
        addentry.bindValue(5, body);
        success=addentry.exec();
    }

    db2.close();
    return success;
}

//################################################################################################
bool MySQLCore::DeleteEntry(QString id){

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="DELETE FROM entries WHERE id=" + id;
    QSqlQuery q(query,db2);

    bool success=q.exec();
    db2.close();
    return success;
}
//################################################################################################
// Get day and year based on entry
QString MySQLCore::TimeStamp(QString id){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT month,day,year FROM entries WHERE id=" + id;

    QSqlQuery q(query,db2);

    q.exec();
    QString month;
    QString day;
    QString year;
    QString timestamp;

    while(q.next()){
        QVariant Vmonth=q.value(0);
        month=Vmonth.toString();

        QVariant Vday=q.value(1);
        day=Vday.toString();

        QVariant Vyear=q.value(2);
        year=Vyear.toString();

        QString format;

        switch(Buffer::date_format){
        case 0: // European

            if(Buffer::use_dow){
                format="dddd, d/M/yyyy";
            }
            else{
                format="d/M/yyyy";
            }

            break;

        case 1: // USA Style
            if(Buffer::use_dow){
                format="dddd, M/d/yyyy";
            }
            else{
                format="M/d/yyyy";
            }
            break;
        case 2: // ISO

            if(Buffer::use_dow){
                format="dddd, yyyy/M/d";
            }
            else{
                format="yyyy/MM/dd";
            }

            break;
        }

        QDate date;
        date.setYMD(year.toInt(),month.toInt(),day.toInt());
        format=format.simplified();
        timestamp=date.toString(format);


        //cout << entry.toStdString() << endl;
    }

    db2.close();

    return timestamp;
}

//################################################################################################
// Function that retrieves entry $index from the database and
// returns data from "body" as a QString. This is meant to be called from the EntryList tree and
// index should be the value from the second hidden column.
QString MySQLCore::RetrieveEntry(QString id){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT body FROM entries WHERE id=" + id;
    //cout << query.toStdString() << endl;
    QSqlQuery q(query,db2);

    q.exec();
    QString entry;
    while(q.next()){
        QVariant result=q.value(0);
        entry=result.toString();
        //cout << entry.toStdString() << endl;
    }

    db2.close();

    return entry;
}


//################################################################################################
/*

 Retrieve full entry from database to populate Editor form. This function is ONLY
 used for update functions.
  */

QList<QString> MySQLCore::RetrieveEntryFull(QString id){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT title, month, day, year, tags, body FROM entries WHERE id=" + id;
    //cout << query.toStdString() << endl;

    QSqlQuery q(query,db2);

    q.exec();

    db2.close();

    QList<QString> EntryArray;

    while(q.next()){
        QVariant t=q.value(0);
        QString title=t.toString();
        QString month=q.value(1).toString();
        QString day=q.value(2).toString();
        QString year=q.value(3).toString();
        QString tags=q.value(4).toString();
        QString body=q.value(5).toString();

        EntryArray.append(title);
        EntryArray.append(month);
        EntryArray.append(day);
        EntryArray.append(year);
        EntryArray.append(tags);
        EntryArray.append(body);

    }

    return EntryArray;
}

//################################################################################################
// Get the most recent entry from the db and return it as a QString
QString MySQLCore::Recent(){

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();
    //db.open();
    QString q="SELECT id,body FROM entries ORDER BY id DESC";
    QSqlQuery get(q,db2);

    get.exec();
    get.first();
    QVariant result=get.value(1);

    QVariant id=get.value(0);

    QString body=result.toString();
    ID=id.toInt();


    recordnum=id.toString();

    db2.close();

    return body;
}
//################################################################################################
QString MySQLCore::GetPrevious(){

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    ID--;
    QString q="SELECT body FROM entries where id=?";

    QSqlQuery get(q,db2);
    get.bindValue(0, ID);
    get.exec();
    QVariant result=get.value(0);
    QString body=result.toString();

    db2.close();
    return body;

}

//################################################################################################
QList<QString> MySQLCore::getYear(){
    using namespace std;
    QList <QString> YearList;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT year FROM entries ORDER BY id DESC";
    QSqlQuery getyear(query,db2);
    getyear.exec();

    int pos=0;
    QString recent;

    // Add all years to YearList
    if(Buffer::allentries){

        while(getyear.next()){

            QVariant result=getyear.value(0);
            QString year=result.toString();

            if(year!=recent){

                //cout << "Added Year: " << year.toStdString() << endl;
                YearList.insert(pos,year);
                pos++;
                recent=year;
            }
        }

    }

    // Only add entries from a specific year range to YearList
    else{
        //cout << "Entry range: " << max_range << endl;
        int range=0;

        while(getyear.next()){

            QVariant result=getyear.value(0);
            QString year=result.toString();

            if(year!=recent){

                //cout << "Added Year: " << year.toStdString() << endl;
                YearList.insert(pos,year);
                pos++;
                range++;

                // force the loop to stop after we get the year range we want
                if(range==Buffer::entryrange){
                    break;
                }

                recent=year;

            }
        }

    }

    db2.close();

    return YearList;
}
//################################################################################################
QList<QString> MySQLCore::getMonth(QString nextyear){
    using namespace std;
    QList <QString> MonthList;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT month FROM entries WHERE year=" + nextyear +  " ORDER BY id DESC";
    QSqlQuery getmonth(query,db2);
    getmonth.exec();


    int pos=0;
    QString recent;
    while(getmonth.next()){
        QVariant result=getmonth.value(0);
        QString month=result.toString();

        if(month!=recent){
            //cout << "Inserting month: " << month.toStdString() << endl;
            MonthList.append(month);
            pos++;
        }
        recent=month;

    }

    db2.close();
    return MonthList;
}
//################################################################################################
QList<QString> MySQLCore::getDay(QString itemmonth, QString nextyear){
    using namespace std;
    QList <QString> DayList;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT day FROM entries WHERE MONTH=" + itemmonth + " AND year=" + nextyear + " ORDER BY id DESC";
    //cout << query.toStdString() << endl;
    QSqlQuery getday(query,db2);
    getday.exec();
    int pos=0;
    QString previous;
    while(getday.next()){
        QVariant result=getday.value(0);
        QString day=result.toString();

        if(day!=previous){
            //cout << "Inserting day: " << day.toStdString() << endl;
            DayList.insert(pos,day);
        }
        pos++;
        previous=day;

    }

    db2.close();

    return DayList;

}
//################################################################################################
QList<QString> MySQLCore::getEntries(QString itemday, QString itemmonth){

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QList <QString> EntryList;

    // Even though we want to display results from newest-> oldest,
    // we sort by ascending here because QList::Insert adds them in reverse order.
    // running ORDER BY id DESC will actually cause entries to be sorted by ascending order.

    QString query="SELECT title,id FROM entries WHERE day=" + itemday + " AND month=" +
            itemmonth + " ORDER BY id ASC";
    QSqlQuery getentry(query,db2);


    getentry.exec();

    int pos=0;

    while(getentry.next()){
        QVariant result=getentry.value(0);
        QString entry=result.toString();

        QVariant rownum=getentry.value(1);
        QString id=rownum.toString();

        QString previous=NULL;

        entry=entry+"|"+id;




        if(entry!=previous){
            EntryList.insert(pos,entry);
        }
        previous=entry;
    }

    db2.close();

    return EntryList;

}
//################################################################################################
// get and process timestamp
QString MySQLCore::GetTimestamp(QString id){

    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT time FROM entries WHERE id=" + id;
    //cout << query.toStdString() << endl;
    QSqlQuery q(query,db2);

    q.exec();
    QString timestamp="test";
    while(q.next()){
        QVariant result=q.value(0);
        timestamp=result.toString();

    }

    // Process null times
    if(timestamp==""){
        timestamp="[unknown time]";
    }
    else{
        // default time is 24 hour style. If user wants AM/PM, we must convert it
        if(!Buffer::use_24_hour){
            QTime time=QTime::fromString(timestamp,"HH:mm");
            timestamp=time.toString("h:mm ap");

        }
    }

    db2.close();
    return timestamp;
}

//################################################################################################
// Get title from database
QString MySQLCore::GetTitle(QString id){
    using namespace std;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT title FROM entries WHERE id=" + id;

    QSqlQuery q(query,db2);

    q.exec();
    QString title;
    while(q.next()){
        QVariant result=q.value(0);
        title=result.toString();
        //cout << entry.toStdString() << endl;
    }

    db2.close();

    return title;
}
//################################################################################################
// Get tags from the database
QString MySQLCore::GetTags(QString id){
    using namespace std;


    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QSqlQuery q("SELECT tags FROM entries WHERE id=" + id,db2);
    q.exec();

    QString tags;
    while(q.next()){
        QVariant result=q.value(0);
        tags=result.toString();
        //cout << entry.toStdString() << endl;
    }

    db2.close();
    return tags;
}
//################################################################################################
// Create aggregated tag list
QList<QString> MySQLCore::TagSearch(){
    using namespace std;
    QList<QString> tags;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QSqlQuery q("SELECT tags FROM entries",db2);
    q.exec();

    QString next_tag;
    while(q.next()){
        QVariant result=q.value(0);
        next_tag=result.toString();

        // add the tag data to QList, excluding NULL entries
        if((next_tag != "null") && (next_tag != "Null")){
            //cout << "Adding: " << next_tag.toStdString() << endl;
            tags.append(next_tag);
        }
    }

    db2.close();
    return tags;
}

//################################################################################################
// This creates the list of all entries in the database so we can browse backward and forward
QList<QString> MySQLCore::Create_ID_List(){
    using namespace std;
    QList <QString> IDList;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();


    QString query="SELECT id FROM entries ORDER BY id ASC";

    QSqlQuery id(query,db2);
    id.exec();

    int pos=0;
    while(id.next()){
        QVariant result=id.value(0);
        QString entry=result.toString();
        //cout << entry.toStdString() << endl;
        IDList.insert(pos, entry);
        pos++;
    }

    db2.close();

    return IDList;
}

//################################################################################################
/*

  MySQLcore::getEntriesMonth() is only called from MainWindow if Buffer::sortbyday==false.
  This function sorts all entries by the month they were made and returns a list.
  The default RJ behavior is to sort entries by month AND day instead.

*/


QList<QString> MySQLCore::getEntriesMonth(QString month, QString year){
    using namespace std;
    QList <QString> EntryList;

    QSqlDatabase db2=QSqlDatabase::database("@mysql");
    db2.open();

    QString query="SELECT id,title,day FROM entries WHERE month=" +
            month + " AND year="+ year +" ORDER BY ID DESC";
    QSqlQuery b(query,db2);

    b.exec();

    int pos=0;
    while(b.next()){
        QVariant id=b.value(0);
        QString StrID=id.toString();

        QVariant title=b.value(1);
        QString StrTitle=title.toString();

        QVariant day=b.value(2);
        QString StrDay=day.toString();

        QString item=StrID + "," + StrTitle + "," + StrDay;
        EntryList.insert(pos, item);
        pos++;
    }

    db2.close();
    return EntryList;
}

//################################################################################################
/*
  *THE* most important function, for without this none of the other MySQL commands are of any use whatsoever
  unless you know how to set everything up yourself. This function uses the root account on $host to
  create the $journal database, create the ENTRIES table in $database, and create a new user account with access
  permissions for $journal. Wow.
  */

bool MySQLCore::CreateDatabase(QString host,QString root_pass, QString db_name,
                               QString port, QString newuser, QString newuser_pass,
                               bool on_remote_host)
{

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL", "@create");
    using namespace std;

    // Bugfix 8/21/12: Save old login data in case login fails. That way other connections won't get broken after
    // exiting JournalSelector
    old_username=db2.userName();
    old_password=db2.password();

    bool success=true; //success should be true unless something goes wrong and changes it to false
    bool exists=false;

    int db_port=port.toInt();


    db2.setHostName(host);
    db2.setPort(db_port);
    db2.setUserName("root");
    db2.setPassword(root_pass);
    db2.open();


    QSqlQuery duplicate_check("SHOW DATABASES",db2);
    duplicate_check.exec();


    while(duplicate_check.next()){
        QVariant b=duplicate_check.value(0);
        QString output=b.toString();
        //cout << "OUTPUT: This is the schema check: " << output.toStdString() << endl;

        // display a dialog if the database exists already.
        if(db_name==output){
            success=false;
            exists=true;
            break;
        }
    }
    db2.close();

    // only proceed if the other database does not exist
    if(!exists){
        db2.open();
        // create database
        QSqlQuery create_db("CREATE DATABASE IF NOT EXISTS " + db_name,db2);
        create_db.exec();

        db2.close();

        db2.setDatabaseName(db_name);
        db2.open();
        // create the entries table now

        QSqlQuery create_entries("CREATE TABLE entries(id int not null auto_increment, "
                                 "primary key (id), title VARCHAR(500), month VARCHAR(2), "
                                 "day VARCHAR(2), year VARCHAR(4), tags VARCHAR(300), "
                                 "body TEXT, time VARCHAR(5))",db2);

        create_entries.exec();
        db2.close();

        // Grant rights to new user
        db2.open();
        QString grant="GRANT SELECT, INSERT, DROP, UPDATE, DELETE ON " + db_name +
                ".* TO '" + newuser + "'@'" + host + "' IDENTIFIED BY '" + newuser_pass + "'";
        QSqlQuery grant_rights(grant,db2);
        grant_rights.exec();


        // 0.5 Bugfix: Grant persissions to user if database is located on remote host --Will Kraft (6/7/14).
        if(on_remote_host){

             QList<QHostAddress> if_list = QNetworkInterface::allAddresses();
             QStringList ipaddr;

             // Get a list of all ip addresses on the system and save the ones that are for LAN (192.168.x.y)
             for(int nIter=0; nIter<if_list.count(); nIter++)
             {
                   QString addr=if_list[nIter].toString();

                   if(addr.contains("192.168."))
                       ipaddr << addr;
             }

             // Grant permessions to every IP in the list so the user wil lbe able to access their journal from
             // the current remote computer.

             for(int i=0; i<ipaddr.count(); i++){

                 host=ipaddr.at(i);

                 QSqlQuery grant_remote_rights("GRANT SELECT, INSERT, DROP, UPDATE, DELETE ON `" + db_name +
                                               "`.* TO '" + newuser + "'@'" + host + "' IDENTIFIED BY '" +
                                               newuser_pass + "'; FLUSH PRIVILEGES",db2);
                 grant_remote_rights.exec();

                 cout << "OUTPUT: Granting rights to remote database '" << newuser.toStdString() << "'@'"
                      << host.toStdString() << "'." << endl;

                 qDebug() << grant_remote_rights.lastError();
             }
        }

        // Flush the privileges so the new Grants start working
        QSqlQuery flush("FLUSH PRIVILEGES",db2);
        flush.exec();


        db2.close();
    }

    // show warning if database exists
    if(exists){
        cout << "OUTPUT: " << db_name.toStdString() << " already exists! The original database was not replaced." << endl;
        QMessageBox s;
        s.critical(MainWindow::mw,"RoboJournal","<b>" + db_name + "</b> already exists on <b>" + host +
                   "</b>! The existing <b>" + db_name + "</b> database was not changed or replaced.");
        return true;
    }


    // show an error if we failed to connect
    if(db2.isOpenError()){
        cout << "OUTPUT: RoboJournal could not connect. Are you using the right username and password?" << endl;
        return false;
    }

    // If there's no MySQL, display an error
    if(!db2.isDriverAvailable("QMYSQL")){
        cout << "OUTPUT: MySQL support was not found! MySQL functionality disabled." << endl;
        QMessageBox a;
        a.critical(MainWindow::mw,"RoboJournal","RoboJournal could not find or use the Qt MySQL driver. "
                   "This problem occurs if the Qt environment used to compile RoboJournal was built without MySQL support.");
        return false;
    }

    // if everything went ok provide feedback.
    if(success){

        cout << "OUTPUT: Rights granted to new user " << newuser.toStdString() <<
                " on journal " << db_name.toStdString() << endl;

        cout << "OUTPUT: Successfully created new journal "  << db_name.toStdString() << endl;
    }

    // if login fails, restore the old values before closing the database. That way, the connections in the rest of
    // the app won't get broken.
    if(!success){
        db2.setUserName(old_username);
        db2.setPassword(old_password);
        // clean up
        old_password.clear();
        old_username.clear();
    }

    // Bugfix (8/19/13) Change db2's association so we can remove it without creating warnings.
    db2 = QSqlDatabase();
    db2.removeDatabase("@create");

    return success;
}
