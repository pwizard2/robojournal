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

#include "configurationmysql.h"
#include "ui_configurationmysql.h"
#include "core/buffer.h"
#include "ui/newconfig.h"
#include "core/favoritecore.h"
#include <QList>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QTreeWidgetItemIterator>
#include <sql/mysqlcore.h>

ConfigurationMySQL::ConfigurationMySQL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationMySQL)
{
    ui->setupUi(this);
    PopulateForm();


}

ConfigurationMySQL::~ConfigurationMySQL()
{
    delete ui;
}

void ConfigurationMySQL::on_DefaultPort_clicked()
{
    ui->Port->setText("3306");
    ui->DefaultPort->setDisabled(true);
}

void ConfigurationMySQL::on_Port_textChanged(const QString &arg1)
{
    if(arg1 != "3306"){
        ui->DefaultPort->setDisabled(false);
    }
    else{
        ui->DefaultPort->setDisabled(true);
    }
}

//###################################################################################################
void ConfigurationMySQL::PopulateForm(){

    ui->Username->setText(Buffer::defaultuser);
    ui->DefaultHost->setText(Buffer::defaulthost);
    ui->Port->setText(QString::number(Buffer::databaseport));
    ui->Database->setText(Buffer::defaultdatabase);

    ui->AllowRoot->setChecked(Buffer::allowroot);

    // Bugfix: Set has_displayed variable to false so the empty journal list message gets displayed once
    //if the journal list is empty -- Will Kraft (11/17/13).
    has_displayed=false;

    // Suppress message for ApplyDefaultProperties. --Will Kraft (2/16/14).
    load_up=true;

    Show_Known_Journals();

    load_up=false;

}

//###################################################################################################
//New for 0.5. Get the checked state of each journal in the Known Journals List (7/20/13).
void ConfigurationMySQL::Harvest_Favorite_Databases(){

    QTreeWidgetItemIterator it(ui->KnownJournals);
    FavoriteCore f;

    while(*it){

        QTreeWidgetItem *current=*it;

        if(Qt::Checked == current->checkState(0)){
            f.setFavorite(current->text(2),true);
        }
        else{
            f.setFavorite(current->text(2),false);
        }

        ++it;
    }
}

//###################################################################################################
// New for 0.5. Create the list of known journals (7/19/13).
void ConfigurationMySQL::Show_Known_Journals(){

    QIcon db(":/icons/database.png");

    ui->KnownJournals->setHeaderHidden(false);
    ui->KnownJournals->resizeColumnToContents(0);
    ui->KnownJournals->resizeColumnToContents(1);
    ui->KnownJournals->resizeColumnToContents(3);

    ui->KnownJournals->setColumnHidden(2,true);

    // Hide range column for now. Independent range infrastructure is in place for this release but I'm going to wait until
    // 0.6 to implement it because it would be too many new features at once. --Will Kraft (10/2/13).
    ui->KnownJournals->setColumnHidden(3,true);

    ui->KnownJournals->sortByColumn(0, Qt::AscendingOrder);
    ui->KnownJournals->setRootIsDecorated(0);

    FavoriteCore f;
    QList<QStringList> journals=f.getKnownJournals();

    if(journals.isEmpty()){

        QMessageBox m;
        m.information(this,"RoboJournal","The list of known MySQL-based journals is currently empty. This problem "
                      "should automatically resolve itself the next time you make a connection or use the Journal Selector.");
        return;
    }
    else{

        for(int i=0; i < journals.size(); i++){
            QStringList row=journals.at(i);
            QTreeWidgetItem *new_item=new QTreeWidgetItem(ui->KnownJournals);
            new_item->setIcon(0,db);
            new_item->setText(0,row.at(1));
            new_item->setText(1,row.at(2));
            new_item->setText(2,row.at(0));
            new_item->setText(3,row.at(5));

            new_item->setToolTip(0,"Check this item to select it as a favorite.");
            new_item->setToolTip(1,"Check this item to select it as a favorite.");

            new_item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

            switch(row.at(4).toInt()){
            case 0:
                new_item->setCheckState(0, Qt::Unchecked);
                break;

            case 1:
                new_item->setCheckState(0, Qt::Checked);
                break;
            }


            if(row.at(1)==Buffer::defaultdatabase){
                ApplyDefaultProperties(new_item);
            }
        }
        ui->KnownJournals->resizeColumnToContents(0);
        ui->KnownJournals->resizeColumnToContents(1);
    }


    // Only do database maintenance if there is a password in the buffer since we need it to log in (8/19/13).
    if((!Buffer::password.isEmpty()) && (!journals.isEmpty())){

        QString host=Buffer::host;

        if(host.isEmpty())
            host=Buffer::defaulthost;

        MySQLCore my;
        QStringList dynamic=my.GetDatabaseList(host,QString::number(Buffer::databaseport),Buffer::username,Buffer::password,true);

        if(!dynamic.isEmpty()){

            bool changed=f.Do_Maintenance(journals,dynamic);

            //refresh the journal list if maintenance changes were made
            if(changed){
                ui->KnownJournals->clear();
                Show_Known_Journals();
            }
        }
    }
}

void ConfigurationMySQL::GetChanges(){

    Newconfig::new_default_user=ui->Username->text();
    Newconfig::new_default_db=ui->Database->text();
    Newconfig::new_default_host=ui->DefaultHost->text();
    Newconfig::new_default_port=ui->Port->text();
    Newconfig::new_allow_root=ui->AllowRoot->isChecked();

    Harvest_Favorite_Databases();
}

// Apply special properties to the default database QtreeWidgetItem by bolding the text
// and giving it a gold database icon. Also, make sure it is checked.
// New for 0.5. --Will Kraft 7/19/13.
void ConfigurationMySQL::ApplyDefaultProperties(QTreeWidgetItem *item){

    QFont special;
    special.setBold(true);
    //special.setUnderline(true);
    item->setFont(0,special);

    item->setCheckState(0, Qt::Checked);
    QIcon gold_db(":/icons/database-gold.png");
    item->setIcon(0,gold_db);

    //Bugfix (7/31/13): prevent user from un-checking the box of the current default item.
    item->setFlags(Qt::ItemIsEnabled);

    Buffer::defaultdatabase=item->text(0);
    default_db=item;

    ui->KnownJournals->resizeColumnToContents(0);
    ui->KnownJournals->resizeColumnToContents(1);

    QMessageBox m;

    // Show confirmation of a default journal change. Suppress this message when the form is instantiated.
    if(!load_up)
        m.information(this,"RoboJournal","The default journal has been changed to <b>" +
                      item->text(0) + "@" + item->text(1) + "</b>.");

}

// Demote the old default database whenever a new one is chosen from the list.
// New for 0.5. --Will Kraft (7/20/13)
void ConfigurationMySQL::demoteDatabase(QTreeWidgetItem *item){

    QFont normal;
    normal.setBold(false);
    //special.setUnderline(true);
    item->setFont(0,normal);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

    item->setCheckState(0, Qt::Checked);
    QIcon db(":/icons/database.png");
    item->setIcon(0,db);
}


// New slot that allows the user to easily change the default db by clicking an
// item on the Known Journals list. New for 0.5. --Will Kraft 7/19/13.
void ConfigurationMySQL::on_KnownJournals_itemDoubleClicked(QTreeWidgetItem *item)
{
    QString new_choice=item->text(0);
    QString new_host=item->text(1);
    QMessageBox m;

    if(new_choice != Buffer::defaultdatabase){
        if(Buffer::showwarnings){


            int choice=m.question(this,"RoboJournal","Do you want to set <b>" + new_choice + "@" + new_host +
                                  "</b> as the new default MySQL journal?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            switch(choice){

            case QMessageBox::Yes:
                ui->Database->setText(new_choice);
                ui->DefaultHost->setText(new_host);

                demoteDatabase(default_db);
                ApplyDefaultProperties(item);

                break;

            case QMessageBox::No: // do nothing
                break;
            }
        }
        else{
            ui->Database->setText(new_choice);
            demoteDatabase(default_db);
            ApplyDefaultProperties(item);

        }
    }
    else{
        m.critical(this,"RoboJournal","<b>" + new_choice + "@" + new_host + "</b> is already the default MySQL journal.");
    }
}
