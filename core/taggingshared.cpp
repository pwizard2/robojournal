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
    This class contains tag-related methods that are "shared" between many classes.
    When one of these functions is needed, the app calls the method in this class
    instead of handling everything internally. It makes more sense to design the app
    this way instead of having redundant code.
  */

#include "taggingshared.h"
#include <QStringList>
#include "core/buffer.h"
#include "sql/mysqlcore.h"
#include <QListWidgetItem>

TaggingShared::TaggingShared(){

}

QListWidgetItem TaggingShared::DeclareTag(){

    QListWidgetItem newtag;
    return newtag;
}

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

// Save the tags. This is used when the tags are modified; not when they are created at
// the same time as the entry itself. (6/11/13)
void TaggingShared::SaveTags(QString tags){

}
