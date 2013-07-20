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


#include "ui/dblogin.h"
#include "ui_dblogin.h"
#include "ui/mainwindow.h"
#include "core/buffer.h"
#include <iostream>
#include <QMessageBox>
#include "core/favoritecore.h"
#include "sql/mysqlcore.h"

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

    //New 0.5 code

    ui->buttonBox->setContentsMargins(9,9,9,9);

    if(Buffer::backend=="MySQL"){
        ui->tabWidget->setTabEnabled(1,false);
    }

    if(Buffer::backend=="SQLite"){
        ui->tabWidget->setTabEnabled(0,false);
    }

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
    // Check to see if defaults are being used
    if(Buffer::alwaysusedefaults){
        ui->UseUserDefault->click();
    }

    if(ui->UseUserDefault->isChecked()){
        ui->Password->setFocus(Qt::PopupFocusReason);
    }
}

//###################################################################################################
void DBLogin::on_UseUserDefault_clicked()
{

    if(ui->UseUserDefault->isChecked()){
        ui->Username->setReadOnly(true);
        ui->Username->setText(Buffer::defaultuser);

    }
    else{
        ui->Username->setReadOnly(false);
        ui->Username->clear();
    }
}

void DBLogin::on_UseDBDefault_clicked()
{

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

        // get data from form and pass it to Buffer class
        Buffer::login_succeeded=true;

        Buffer::database_name=ui->WhichDB->currentText();

        Buffer::host=ui->DBHost->currentText();
        Buffer::username=ui->Username->text();
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
}

void DBLogin::on_buttonBox_rejected()
{
    //login_succeeded=0;
    Buffer::login_succeeded=false;
}

//###################################################################################################
// Populate Comboboxes. This method only populates the Host CB because adding the host fields automatically
// triggers a slot that calls RefreshJournalList. New for 0.5. --Will Kraft (7/20/13).
void DBLogin::PopulateComboBoxes(){

    ui->DBHost->clear();
    ui->WhichDB->clear();
    QIcon h(":/icons/server.png");
    FavoriteCore f;
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

}
