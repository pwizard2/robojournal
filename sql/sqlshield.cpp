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

#include "sqlshield.h"
#include "core/buffer.h"
#include <QMessageBox>
#include <QRegExp>
#include <iostream>

SQLShield::SQLShield()
{
}

//##################################################################################################
// Use regexp to detect common injections, use escaping to break them, and return clean queries.
// This does offer some protection but it does NOT make RoboJournal 100% SQL injection proof!!!!
QString SQLShield::Break_Injections(QString input){
    using namespace std;

    QString sanitized=input;

    // easy stuff first: break quotes for sql injection
    sanitized=sanitized.replace("';","/'/;",Qt::CaseInsensitive);
    sanitized=sanitized.replace(";","/;/" ,Qt::CaseInsensitive);

    sanitized.replace("'; drop","';/drop/", Qt::CaseInsensitive);
    sanitized.replace("' and drop", "'/and/drop", Qt::CaseInsensitive);

    // break quotes anywhere in the statement where column='foo' || column="foo"
    QRegExp quote_statement("\\w+ *= *(\'|\")?.{0,}(\'|\")");
    QStringList quote_match;

    int q_count = 0;
    int q_pos = 0;
    while ((q_pos = quote_statement.indexIn(sanitized, q_pos)) != -1) {
        ++q_count;
        q_pos += quote_statement.matchedLength();
        //cout << "FOUND MATCH: " << check1.cap(0).toStdString() << endl;
        quote_match << quote_statement.cap(0);
    }

    for(int j=0; j < quote_match.size(); j++){
        QString orig=quote_match.at(j);
        //cout << "Match: " << check1_match.at(i).toStdString() << endl;

        QString escaped_quote=orig.replace("\'","//'");
        //cout << "1. CHANGED TO " << escaped_quote.toStdString() << endl;
        sanitized=sanitized.replace(quote_match.at(j),escaped_quote);

        QString escaped_quote2=orig.replace("\"","//\"");
        //cout << "2. CHANGED TO " << escaped_quote2.toStdString() << endl;
        sanitized=sanitized.replace(quote_match.at(j),escaped_quote2);

        QString escaped_quote3=orig.replace("(","//(");
        //cout << "2. CHANGED TO " << escaped_quote2.toStdString() << endl;
        sanitized=sanitized.replace(quote_match.at(j),escaped_quote3);
    }


    //old over-zealous regexp: [\\w*|\\W*][\\']*'* ?;? (DROP|AND|OR|INSERT|UPDATE|DELETE|SELECT|UNION) \\w*
    // old check 1:

    QRegExp check1("\\w?(\'|\"); *(\\w|\\W)*",Qt::CaseInsensitive);

    QStringList check1_match;

    int o_count = 0;
    int o_pos = 0;
    while ((o_pos = check1.indexIn(input, o_pos)) != -1) {
        ++o_count;
        o_pos += check1.matchedLength();
        //cout << "FOUND MATCH: " << check1.cap(0).toStdString() << endl;
        check1_match << check1.cap(0);

    }

    for(int i=0; i < check1_match.size(); i++){
        QString orig=check1_match.at(i);
        //cout << "Match: " << check1_match.at(i).toStdString() << endl;

        QString escaped_quote=orig.replace("\'","//'");
        //cout << "1. CHANGED TO " << escaped_quote.toStdString() << endl;
        sanitized=sanitized.replace(check1_match.at(i),escaped_quote);

        QString escaped_quote2=orig.replace("\"","//\"");
        //cout << "2. CHANGED TO " << escaped_quote2.toStdString() << endl;
        sanitized=sanitized.replace(check1_match.at(i),escaped_quote2);

        QString escaped_quote3=orig.replace(")","//)");
        //cout << "2. CHANGED TO " << escaped_quote2.toStdString() << endl;
        sanitized=sanitized.replace(check1_match.at(i),escaped_quote3);


        cout << "OUTPUT: Blocked possible SQL injection attempt! Shame on you." << endl;
    }

    // stop select, union, or 1=1 queries from showing the entire db
    QRegExp check2("\\w?'; *select *\\w*",Qt::CaseInsensitive);
    sanitized=sanitized.replace(check2,"//';/select");

    QRegExp check3("\\w?'; *1=1");
    sanitized=sanitized.replace(check3,";//' //1=//1");

    QRegExp check4("\\w?'; *union *\\w*",Qt::CaseInsensitive);
    sanitized=sanitized.replace(check4,"\\'; /union");

    QRegExp check5(");");
    sanitized=sanitized.replace(check5,"//);");


    // show sanitized query
    //cout << "Sanitized: " << sanitized.toStdString() << endl;

    return sanitized;
}
