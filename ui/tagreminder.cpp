/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    


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


#include "tagreminder.h"
#include "ui_tagreminder.h"
#include "core/buffer.h"
#include "sql/mysqlcore.h"
#include <QDate>
#include <iostream>
#include <QIcon>
#include "ui/tagger.h"
#include <QMessageBox>
#include "core/settingsmanager.h"
#include <QTimer>
#include <QCloseEvent>





TagReminder::TagReminder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagReminder)
{
    ui->setupUi(this);
    PrimaryConfig();
}

//############################################################################################
TagReminder::~TagReminder()
{
    delete ui;
}

//############################################################################################
// The Reject() slot is called if the user clicks the Close button.
void TagReminder::reject(){

    using namespace std;


    // 3/13/13: Store the current state of the ShowReminder box in the Buffer. This value is then
    // used in the SettingsManager class to update the configuration if necessary.
    if(!ui->ShowReminder->isChecked()){
       Buffer::show_reminder_next_time=false;
    }

    // Hide the dialog because this function breaks the normal close() function.
    this->hide();
}



//############################################################################################
// This is a stealth function that detects the number of untagged entries and returns the value
// as an int. If the int > 0, the form is displayed; otherwise, RoboJournal does nothing with
// the information.

int TagReminder::Lookup(){

    using namespace std;
    int untagged_count=0;

    if(Buffer::backend=="MySQL"){
        MySQLCore n;
        Buffer::records=n.NullSearch();
        untagged_count=Buffer::records.size();
        //cout << "Number of records (in Lookup() function): " << untagged_count << endl;
    }

    return untagged_count;
}

//############################################################################################
// Setup form; typical for all RoboJournal classes.
void TagReminder::PrimaryConfig(){

    setWindowTitle("Tag Reminder - RoboJournal");

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    // define header titles for item list. They are displayed in the order they are fed into the StringList.
    ui->EntryList->setColumnCount(3);
    QStringList header;
    header << "Entry Title" << "Date"  << "ID";
    ui->EntryList->setHeaderLabels(header);

    // Hide ID column
    ui->EntryList->setColumnHidden(2,true);

    // hide unnecessary node inset by hiding root. (we have no root node anyway).
    ui->EntryList->setRootIsDecorated(false);

    //show alternating row colors if config allows it
    if(Buffer::alternate_rows){
        ui->EntryList->setAlternatingRowColors(true);
    }
    else{
        ui->EntryList->setAlternatingRowColors(false);
    }

    // 3/26/13: Enable sort functionality
    ui->EntryList->sortByColumn(1,Qt::DescendingOrder);

    // Bugfix 3/13/13: Set the showReminder value to true to begin with. If the user unchecks the ShowReminder box
    // this value gets set to false (and the config gets updated ot reflect this) when the dialog is closed.
    Buffer::show_reminder_next_time=true;

    // Create the list of untagged entries...
    Lookup();

    // ...and display it.
    CreateList();
}

//############################################################################################
// 2/26/13: This function populates the list/treewidget with the results of the MySQL query. This function
// uses the 'records' QList that was populated bythe Lookup() function; this function does NOT
// do another SQL call.

void TagReminder::CreateList(){

    using namespace std;

    QListIterator<QStringList> a(Buffer::records);
    //cout << "Number of items in list: " << records.size() << endl;

    QString iconpath;

    // set node icon according to software preferences
    switch(Buffer::entry_node_icon){
    case 0:
        iconpath=":/icons/pencil.png";
        break;

    case 1:
        iconpath=":/icons/pencil2.png";
        break;

    case 2:
        iconpath=":/icons/pencil-small.png";
        break;

    case 3:
        iconpath=":/icons/balloon-quotation.png";
        break;

    case 4:
        iconpath=":/icons/node.png";
        break;

    case 5:
        iconpath=":/icons/bullet_black.png";
        break;

    case 6:
        iconpath=":/icons/bullet_blue.png";
        break;
    }

    QIcon node(iconpath);

    // create a node for each item in the records Qlist
    while(a.hasNext()){

        QStringList nextitem=a.next();
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->EntryList);

        QString title=nextitem.at(1);
        QString date=nextitem.at(2);
        QString id=nextitem.at(0);

        //Process date from QString into actual QDate object so it sorts correctly.
        QDate realdate;

        QStringList datearray=date.split("-");
        int month, day, year;
        month=datearray.at(0).toInt();
        day=datearray.at(1).toInt();
        year=datearray.at(2).toInt();

        realdate.setDate(year,month,day);


        // add item data to the node columns.
        item->setText(0,title + "  "); // title column
        item->setIcon(0,node);
        item->setData(1,Qt::DisplayRole,realdate);
        item->setText(2,id); // invisible id column; this value is passed to the tagger.
    }

    //Resize columns to eliminate horizontal scroll if possible.
    for(int i=0; i < ui->EntryList->columnCount(); i++){
        ui->EntryList->resizeColumnToContents(i);
    }

    // Set item count message.
    int count=Buffer::records.size();
    QString num=QString::number(count);

    if(count > 1){
        ui->ItemCount->setText(num + " entries need to be tagged.");
    }
    else{
        ui->ItemCount->setText(num + " entry needs to be tagged.");
    }

    // Auto-select the first item in the list to prevent a segfault if the user clicks
    // Manage Tags w/o making a selection first.
    ui->EntryList->setCurrentItem(ui->EntryList->topLevelItem(0));
}

//############################################################################################
void TagReminder::LaunchTagger(){

    if(ui->EntryList->currentItem()->isSelected()){
        Tagger t(this);
        // pass currently selected index to the Tagger class.
        t.id_num=Buffer::reminder_choice;

        t.setWindowTitle(Buffer::reminder_title.trimmed() + " - Manage Tags");
        t.exec();

        // Clean up after the tagger
        Buffer::reminder_title.clear();
        Buffer::reminder_choice.clear();
        t.id_num.clear();
        RefreshList();
    }
    else{
        QMessageBox m;
        m.critical(this,"RoboJournal","Please select an entry to tag.");
    }
}

//############################################################################################
// 2/25/13: force a manual refresh of the tag list. This should only be used after the user tags an entry on the list.
void TagReminder::RefreshList(){

    // purge and recreate the list.
    ui->EntryList->clear();
    Buffer::records.clear();
    Lookup();
    CreateList();

    // auto-close the dialog if there are no more entries to tag.
    if(Buffer::records.count()==0){

        QMessageBox b;
        b.information(this,"RoboJournal","All entries have been successfully tagged. "
                      "RoboJournal will automatically close the Tag Reminder window now.");

        this->accept();
    }
}

//############################################################################################
void TagReminder::on_ManageTags_clicked()
{
    LaunchTagger();
}


//############################################################################################
// This slot keeps the static values for id number and title syncronized whenever the user
// changes the current index on the EntryList.
void TagReminder::on_EntryList_itemSelectionChanged()
{
    QTreeWidgetItem *selected=ui->EntryList->currentItem();
    Buffer::reminder_choice=selected->text(2);
    Buffer::reminder_title=selected->text(0);
}




