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


#include "ui/dblogin.h"
#include "ui_dblogin.h"
#include "ui/mainwindow.h"
#include "core/buffer.h"
#include <iostream>
#include <QMessageBox>
#include "core/favoritecore.h"
#include "sql/mysqlcore.h"
#include <QPushButton>

// Will Kraft: 6/1/13
// Stripped out caps lock checker because it caused more problems during build time than it solved.
// The code required direct-linking on some linux distros, which caused huge problems if the makefile wasn't patched beforehand.
// Taking this code out greatly simplifies the build process.

DBLogin::DBLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DBLogin)
{
    ui->setupUi(this);


    // use large icon on Linux
#ifdef unix
    QIcon unixicon(":/icons/robojournal-icon.png");
    this->setWindowIcon(unixicon);

#endif

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    // Add a nifty new "Connect" button b/c the original OK button didn't really make sense in context.
    // The Connect button now has an icon to help differentiate it from the Cancel button (since both start with "c").
    // New for 0.5 -- Will Kraft (7/21/13).
    QIcon plug(":/icons/connect.png");
    QPushButton *connect=ui->buttonBox->addButton(tr("Co&nnect"),QDialogButtonBox::AcceptRole);
    connect->setIcon(plug);
    connect->setDefault(true);

    ui->buttonBox->setContentsMargins(9,9,9,9);

    if(Buffer::backend=="MySQL"){
        ui->tabWidget->setTabEnabled(1,false);

        setWindowTitle("New Connection [MySQL/MariaDB]");
    }

    if(Buffer::backend=="SQLite"){
        ui->tabWidget->setTabEnabled(0,false);
        setWindowTitle("New Connection [SQLite]");
    }


    // Hide SQLite tab in version 0.5. Comment or delete the next line when 0.6 development starts!
    // --Will Kraft (2/16/14).
    ui->tabWidget->removeTab(1);

    PopulateComboBoxes();
}

//###################################################################################################
DBLogin::~DBLogin()
{
    delete ui;
}

//###################################################################################################
// function that checks if the config has changed
// and refreshes the form if necessary
void DBLogin::ResetPassword(){
    ui->Password->setText(NULL);
}

//###################################################################################################
void DBLogin::Refresh(){

    // Check to see if defaults are being used and fuill in the appropriate data
    if(Buffer::alwaysusedefaults){
        ui->Username->setPlaceholderText(Buffer::defaultuser);
        ui->Password->setFocus();
    }
    else{
        ui->Username->setFocus();
    }
}

//###################################################################################################
void DBLogin::on_buttonBox_accepted()
{

    using namespace std;

    // triggered if the user tries to log in as root
    if(ui->Username->text()=="root"){
        // decide what to do depending on configuration
        if(Buffer::allowroot){
            QMessageBox b;
            int choice=b.question(this,"RoboJournal","Logging in as <b>root</b> can be dangerous. Are you sure you want to do this?",
                                  QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
            if(choice==QMessageBox::Yes){
                // get data from form and pass it to Buffer class
                Buffer::login_succeeded=true;
                Buffer::database_name=ui->WhichDB->currentText();
                Buffer::host=ui->DBHost->currentText();
                Buffer::username=ui->Username->text();
                Buffer::password=ui->Password->text();

            }
            else{
                // stop the login process if the buffer has previous login data
                Buffer::login_succeeded=false;
                this->reject();
            }

        }
        else{
            QMessageBox m;
            m.critical(this,"RoboJournal","You are not allowed to log in as <b>root</b>. Please enter a different username "
                       "and try again.");

            // stop the login process if the buffer has previous login data
            Buffer::login_succeeded=false;
            this->reject();
        }
    }

    // process normal logins
    else{

        if(Buffer::backend=="MySQL"){
            // get data from form and pass it to Buffer class
            Buffer::login_succeeded=true;

            Buffer::database_name=ui->WhichDB->currentText();

            Buffer::host=ui->DBHost->currentText();

            // For the sake of simplicity, use placeholder text for the default value. New for 0.5 (7/29/13).
            if(ui->Username->text().isEmpty()){
                Buffer::username=ui->Username->placeholderText();
            }
            else{
                Buffer::username=ui->Username->text();
            }

            Buffer::password=ui->Password->text();

            // Create/maintain Favorite DB list during each login. This is necessary because it allows
            // RoboJournal to do maintenance behind the scenes in case a new DB gets added or one
            // gets dropped, etc. New for 0.5. --Will Kraft (7/20/13).

            MySQLCore m;

            QString port=QString::number(Buffer::databaseport);
            QStringList journals=m.GetDatabaseList(Buffer::host,port,Buffer::username,Buffer::password,true);
            journals.sort();

            FavoriteCore f;

            if(!journals.isEmpty()){
                for(int i=0; i < journals.size(); i++){
                    QString database=journals.at(i);
                    f.Add_to_DB(database,Buffer::username,Buffer::host);
                }
            }
        }

        if(Buffer::backend=="SQLite"){
            QString choice=ui->SQLiteJournals->currentItem()->text();
            Buffer::database_name=choice;

        }
    }

    // Save the current MariaDB host and database values (12/28/13).
    Buffer::last_db=ui->WhichDB->currentIndex();
    Buffer::last_host=ui->DBHost->currentIndex();
    Buffer::remember_last=true;
}

void DBLogin::on_buttonBox_rejected()
{
    //login_succeeded=0;
    Buffer::login_succeeded=false;

    // Clear saved DBLogin values from Buffer because we don't need to remember them anymore (12/28/13).
    Buffer::last_db = -1;
    Buffer::last_host = -1;
    Buffer::remember_last=false;

}

//###################################################################################################
// Populate Comboboxes. This method only populates the Host CB because adding the host fields automatically
// triggers a slot that calls RefreshJournalList. New for 0.5. --Will Kraft (7/20/13).
void DBLogin::PopulateComboBoxes(){

    FavoriteCore f;

    // Original MySQL-specific code.
    if(Buffer::backend=="MySQL"){
        ui->DBHost->clear();
        ui->WhichDB->clear();
        QIcon h(":/icons/server.png");

        QStringList hosts=f.GetHosts();

        if(hosts.isEmpty()){
            ui->DBHost->addItem(h,Buffer::defaulthost);
        }
        else{
            ui->DBHost->addItems(hosts);

            for (int g=0; g< hosts.size(); g++){
                ui->DBHost->setItemIcon(g,h);
            }

            // Select default host and DB by default.
            for(int i=0; i < hosts.size(); i++){

                if(hosts.at(i)==Buffer::defaulthost){
                    ui->DBHost->setCurrentIndex(i);
                    break;
                }
            }
        }
    }

    // SQLite code added 9/21/13.
    if(Buffer::backend=="SQLite"){

        QIcon db(":/icons/database.png");
        QStringList databases=f.GetSQLiteFavorites();

        for(int i=0; i<databases.size(); i++){

            QString item_name=databases.at(i);
            QListWidgetItem *item=new QListWidgetItem(ui->SQLiteJournals);
            item->setText(item_name);
            item->setIcon(db);

            if(item_name==Buffer::sqlite_default){
                ui->SQLiteJournals->setCurrentItem(item);
                QIcon gold_db(":/icons/database-gold.png");
                item->setIcon(gold_db);
            }
        }
    }

    // Restore most-recently used values if login was unsuccessful (12/28/13).
    if((Buffer::remember_last) && (Buffer::last_db != -1) && (Buffer::last_host != -1)){
        ui->DBHost->setCurrentIndex(Buffer::last_host);
        ui->WhichDB->setCurrentIndex(Buffer::last_db);
    }
}

//###################################################################################################
// Refresh the current journal list when the user switches to a different host.
// This should only be used when DBHost emits a currentIndexChanged. New for 0.5 --Will Kraft(7/20/13).
void DBLogin::RefreshJournalList(QString host){

    ui->WhichDB->clear();
    FavoriteCore f;
    QIcon db(":/icons/database.png");
    QStringList journals=f.GetFavorites(host);

    if(journals.isEmpty()){
        ui->WhichDB->addItem(db,Buffer::defaultdatabase);
    }
    else{
        ui->WhichDB->addItems(journals);

        for (int i=0; i< journals.size(); i++){
            ui->WhichDB->setItemIcon(i,db);
        }

        // Select default DB by default.

        for(int j=0; j < journals.size(); j++){
            if(journals.at(j)==Buffer::defaultdatabase){
                ui->WhichDB->setCurrentIndex(j);
                break;
            }
        }
    }
}

//###################################################################################################

void DBLogin::on_DBHost_currentIndexChanged(const QString &arg1)
{
    RefreshJournalList(arg1);
    ui->Password->setFocus();
}


//###################################################################################################
void DBLogin::on_WhichDB_currentIndexChanged()
{
    ui->Password->setFocus();
}
