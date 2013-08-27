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

    --Will Kraft, 6/11/13. New for Version 0.5.
    This class contains tag-related functions that are "shared" between many classes.
    When one of these functions is needed, the app calls the method from this class
    instead of handling everything internally. It made more sense to design the app
    this way instead of having redundant code.
  */

#include <iostream>
#include "taggingshared.h"
#include <QStringList>
#include "core/buffer.h"
#include "sql/mysqlcore.h"
#include <QInputDialog>
#include <QMessageBox>
#include "sql/sqlshield.h"
#include "ui/mainwindow.h"

//#########################################################################################################
TaggingShared::TaggingShared(){

}

//#########################################################################################################
// 6/13/13: Define a new tag if it is not already in the current/available tags list.
// DefineTag() is meant to be used with the tagging system in several different classes.
// A new tag is declared ONLY if it is not already in the ExistingTags stringlist.
// If the tag is good, it is returned as a QString. Invalid tags are returned as empty
// QStrings. (All referring method(s) are supposed to have safeguards that prevent empty strings from being used).
QString TaggingShared::DefineTag(QStringList ExistingTags){

    QInputDialog d;
    QString tag=d.getText(MainWindow::mw, "RoboJournal", "Enter the new tag:", QLineEdit::Normal);

    tag=tag.trimmed();
    tag=tag.simplified();

    // Bugfix 7/13/13:
    // Remove semicolons from raw tag data because those are used to delimit tags in the database table rows.
    tag=tag.remove(";");

    // Break potential SQL injections so an attacker won't be able to nuke the database.
    // 0.5 Bugfix -- Will Kraft, 6/23/13
    SQLShield s;
    tag=s.Break_Injections(tag);

    bool goodtag=true;
    QMessageBox m;

    // only proceed if the user clicked ok; cancel returns a null string
    if(!tag.isEmpty()){

        if(ExistingTags.contains(tag)){
            goodtag=false;

            // the list isn't called the Available tags list any more in 0.5.
            m.critical(MainWindow::mw,"RoboJournal","\"" + tag + "\" is already on the tags list.");

            tag.clear();
        }

        // Bugfix for 0.4.1 (3/5/13): Replace simple operator check with a "smarter" regexp.
        // The user should NEVER be allowed to declare "null" (case insensitive) as a tag
        // because that is a reserved word in the tagging system; entries marked with Null have
        // "No tags for this post" as their tag data.
        QRegExp banned("null", Qt::CaseInsensitive);

        if(banned.exactMatch(tag)){
            goodtag=false;
            m.critical(MainWindow::mw,"RoboJournal","You are not allowed to declare \"" + tag +
                       "\" (or any other uppercase/lowercase variant of it) because it is a reserved keyword.");
            tag.clear();
        }
    }

    return tag;
}

//#########################################################################################################
// 6/11/13: This method returns a list of all tags in the database. The output is used to
// populate tag lists in several different classes (Mainwindow, Tagger, and EditorTagManager).
QStringList TaggingShared::TagAggregator(){

    QStringList tag_list; // list that holds all existing tags. Each tag should only be listed ONCE.

    if(Buffer::backend=="MySQL"){
        MySQLCore b;

        QList<QString> tags=b.TagSearch();
        QListIterator<QString> i(tags);

        while(i.hasNext()){
            QString line=i.next();
            QStringList tag_array=line.split(";");

            for(int x=0; x<tag_array.size(); x++){

                QString nexttag=tag_array.at(x);

                // 0.5 Bugfix (6/11/13): Trim the tags to remove whitespace and line breaks. This kind
                // of debris sometimes gets added after a database is resored from a MySQLDump. Failing
                // to clean raw tag data can do strange things to the tag list.
                nexttag=nexttag.trimmed();

                // only append to tag_list if it doesn't already contain tag_array[x]
                if(!tag_list.contains(nexttag)){
                    tag_list.append(nexttag);
                }
            }
        }

        tag_list.sort();
    }

    return tag_list;
}

//#########################################################################################################
// Get the tags for entry "id" from the database.(6/11/13)
QStringList TaggingShared::FetchTags(QString id){

    QStringList t_array;

    if(Buffer::backend=="MySQL"){
        MySQLCore a;
        QString tags=a.GetTags(id);

        t_array=tags.split(";",QString::SkipEmptyParts);
    }

    return t_array;
}

//#########################################################################################################
// Save the tags. This method is used when the tags are modified through the Tagger class (7/13/13).
// Tags declared/changed through the Editor are saved with the rest of the entry data.
void TaggingShared::SaveTags(QString tags, QString id){
    using namespace std;

    if(Buffer::backend=="MySQL"){
        MySQLCore m;
        bool success=m.UpdateTags(tags,id);

        if(success){
            cout << "OUTPUT: Tag data updated successfully." << endl;
        }
        else{
            cout << "ERROR: Tag data failed to update!" << endl;
        }
    }
}
