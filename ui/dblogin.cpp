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


}

DBLogin::~DBLogin()
{
    delete ui;
}


// function that checks if the config has changed
// and refreshes the form if necessary

void DBLogin::ResetPassword(){
    ui->Password->setText(NULL);
}

void DBLogin::Refresh(){
    // Check to see if defaults are being used
    if(Buffer::alwaysusedefaults){
        ui->UseDBDefault->click();
        ui->UseUserDefault->click();
    }

    if(ui->UseDBDefault->isChecked() && ui->UseUserDefault->isChecked()){
        ui->Password->setFocus(Qt::PopupFocusReason);
    }

}



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
    if(ui->UseDBDefault->isChecked()){
        ui->DBHost->setReadOnly(true);
        ui->WhichDB->setReadOnly(true);
        ui->DBHost->setText(Buffer::defaulthost);
        ui->WhichDB->setText(Buffer::defaultdatabase);

    }
    else{
        ui->DBHost->setReadOnly(false);
        ui->WhichDB->setReadOnly(false);
        ui->DBHost->clear();
        ui->WhichDB->clear();
    }
}

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
                Buffer::database_name=ui->WhichDB->text();
                Buffer::host=ui->DBHost->text();
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
        Buffer::database_name=ui->WhichDB->text();
        Buffer::host=ui->DBHost->text();
        Buffer::username=ui->Username->text();
        Buffer::password=ui->Password->text();
    }
}

void DBLogin::on_buttonBox_rejected()
{

    //login_succeeded=0;
    Buffer::login_succeeded=false;
}
