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


#include "ui/journalcreator.h"
#include "ui_journalcreator.h"
#include <iostream>
#include <QMessageBox>
#include "sql/mysqlcore.h"
#include "core/buffer.h"
#include "core/settingsmanager.h"
#include <QPushButton>
#include "ui/firstrun.h"
#include "ui_firstrun.h"


JournalCreator::JournalCreator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JournalCreator)
{
    ui->setupUi(this);

#ifdef unix
    QIcon unixicon(":/icons/robojournal-icon.png");
    this->setWindowIcon(unixicon);

#endif

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    PrimaryConfig();


}

JournalCreator::~JournalCreator()
{
    delete ui;
}

// Validate Form when OK is clicked
bool JournalCreator::Validate(){

    bool hasJournalname=true;
    bool hasHostname=true;
    bool hasUsername=true;
    bool hasRoot=true;
    bool hasPort=true;

    // this involves checking to make sure that all fields are filled in properly.
    // if all the bools return true, then the form is valid

    if(ui->JournalName->text().isEmpty()){
        QMessageBox b;
        b.critical(this,"RoboJournal","You must provide a name for this journal!");
        ui->TabBox->setCurrentIndex(1);
        ui->JournalName->setFocus();
        hasJournalname=false;
    }

    if(ui->Hostname->text().isEmpty() && ui->Hostname->placeholderText().isEmpty()){
        QMessageBox b;
        b.critical(this,"RoboJournal","You must specify a hostname! (e.g. localhost)");
        ui->TabBox->setCurrentIndex(1);
        ui->Hostname->setFocus();
        hasHostname=false;
    }

    if(ui->Username->text().isEmpty()){
        QMessageBox b;
        b.critical(this,"RoboJournal","You must specify a username!");
        ui->TabBox->setCurrentIndex(1);
        ui->Username->setFocus();
        hasUsername=false;
    }

    if(ui->rootPassword->text().isEmpty()){
        QMessageBox b;

        QString host=ui->Hostname->text();

        if(ui->Hostname->text().isEmpty()){
            host="localhost";
        }

        b.critical(this,"RoboJournal","You must provide the MySQL root password for <b>" + host + "</b>!");
        ui->TabBox->setCurrentIndex(2);
        ui->rootPassword->setFocus();
        hasRoot=false;
    }

    if(ui->Port->text().isEmpty()){
        QMessageBox b;

        QString host=ui->Hostname->text();

        if(ui->Hostname->text().isEmpty()){
            host="localhost";
        }

        b.critical(this,"RoboJournal","You must specify the correct port for MySQL on <b>" + host + "</b>!");
        ui->TabBox->setCurrentIndex(2);
        ui->Port->setFocus();
        hasPort=false;
    }

    bool valid=false;
    if(hasJournalname && hasHostname && hasUsername && hasRoot && hasPort){
        valid=true;
    }
    else{
        valid=false;
    }

   return valid;

}

void JournalCreator::PrimaryConfig(){

    ShowDescription(1);
    ui->DBType->setCurrentIndex(1);


    // check for firstrun and force certain options if true
    if(Buffer::firstrun){
        ui->UseAsDefault->setDisabled(true);
        ui->UseAsDefault->setChecked(true);
    }


    //ui->Authentication->setHidden(true);

    //ui->Authentication->setDisabled(true);

    //ui->BasicSettings->setDisabled(true);

    // disable the ok button when the form loads
    QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);



    ui->MatchNotify->clear();

    // for now, disable other options until SQLite module is finished
    ui->DBType->setDisabled(true);

}

// Journal creation function is called if Validate() returns TRUE
void JournalCreator::CreateJournal(){
    using namespace std;
    bool valid=Validate();

    // only create database if validation returns true
    if(valid){

        QString host;
        if(ui->Hostname->text()==NULL){
            host=ui->Hostname->placeholderText();
        }
        else{
            host=ui->Hostname->text();
        }

        //cout << host.toStdString();

        QString newuser=ui->Username->text();
        QString db_name=ui->JournalName->text();
        QString root_pass=ui->rootPassword->text();
        QString newuser_pass=ui->Password->text();
        QString port=ui->Port->text();



        // mysql mode
        if(ui->DBType->currentIndex()==1){
            MySQLCore m;
            bool success=m.CreateDatabase(host,root_pass,db_name,port,newuser,newuser_pass);

            QMessageBox m2;
            if(success){

                m2.information(this,"RoboJournal", "The journal <b>" + db_name +
                               "</b> has been successfully created on <b>" + host +
                               "</b>. You may now access this journal with the password you set up for user <b>" +
                               newuser + "</b>.<br><br>Click <b>Connect</b> on the main window to start working with "
                               "your new journal.");

                // Post-setup: Create configuration file with new values if option is checked
                if(ui->UseAsDefault->isChecked()){

                    SettingsManager sm;
                    sm.NewConfig(host,db_name,port,newuser);
                }

                // clear firstrun flag and close the firstrun form
                Buffer::firstrun=false;
                this->close();

            }
            else{
                m2.critical(this,"RoboJournal", "Journal creation attempt failed on <b>" + host + "</b>. Please make sure the root password is"
                            " correct and the host is configured properly and then try again.");

            }
        }

        // SQLite block goes here when finished
    }

}

// check to see if the passwords match
void JournalCreator::PasswordMatch(){


    if((ui->Password->text() == ui->Password2->text()) && (ui->Password->text().length() > 0)){
        ui->MatchNotify->setText("Passwords match");
        ui->MatchNotify->setStyleSheet("color: green");
        QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(true);
    }
    else{

        ui->MatchNotify->setStyleSheet("color: red");

        if(ui->Password->text().length()==0 && ui->Password2->text().length()==0){
            ui->MatchNotify->setText("Passwords are too short");
        }
        else{
            ui->MatchNotify->setText("Passwords do not match");
        }

        QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(false);

    }
}

// Fill in some slots based on system values. User can change these later if they want
void JournalCreator::FillIn(){

    ui->Port->setText("3306");

    // get username from OS and truncate whitespace
    QString sysuser=getenv("USERNAME");

    if(sysuser.isEmpty()){
        sysuser=getenv("USER");
    }

    // bugfix to prevent username from coming up blank on linux systems
#ifdef unix
    sysuser=getenv("USER");
#endif


    sysuser=sysuser.trimmed();
    sysuser=sysuser.replace(" ","_");
    sysuser=sysuser.toLower();


    ui->Username->setText(sysuser);
    ui->JournalName->setText(sysuser + "_journal");
    ui->MatchNotify->setText("Passwords do not match");
    ui->MatchNotify->setStyleSheet("color: red");
}

// Clear values when necessary
void JournalCreator::Clear(){
    ui->Hostname->clear();
    ui->Password->clear();
    ui->Password2->clear();
    ui->JournalName->clear();
    ui->rootPassword->clear();
    ui->Port->clear();
    ui->Username->clear();
    ui->MatchNotify->clear();
}


void JournalCreator::ShowDescription(int index){
    switch(index){
    case 0:
        ui->Description->setHtml("SQLite is a lightweight database that is ideal "
        "for new users or those not very familiar with database management. SQLite databases are stored on this "
        "computer as files and can be easily backed up or replicated. Unlike MySQL, no background process is "
        "required to use SQLite on this machine.<br><br><b>The SQLite journal creation process is automated and requires"
        " no additional input. Click OK to continue.</b>");
        break;

    case 1:
        ui->Description->setHtml("MySQL is a database solution that is renowned for scalability and power, "
        "yet is still fairly easy to configure and use. Journals made from MySQL databases can be stored on remote "
        "servers or on the local computer. Keep in mind that running a MySQL database "
        "on your computer requires that you run a MYSQL sever instance as well. (unlike SQLite which is entirely self-contained) "
        "<br><br>If you want to store your journal on a remote system, you must know how to configure the MySQL process on that machine "
        "to accept remote connections or you will not be able to connect. Because of these required skills, new users or those not "
        "familiar with database administration should not use MySQL.");
        break;
    }
}

void JournalCreator::on_DBType_currentIndexChanged(int index)
{
    ShowDescription(index);

    if(index==0){
       ui->Authentication->setDisabled(true);
       ui->BasicSettings->setDisabled(true);

       Clear();
    }
    else{
        ui->Authentication->setEnabled(true);
        ui->BasicSettings->setEnabled(true);

        FillIn();
    }
}

void JournalCreator::on_Password2_textChanged()
{

    PasswordMatch();
}

void JournalCreator::on_Password_textChanged()
{

    PasswordMatch();
}

void JournalCreator::on_tabWidget_currentChanged()
{

//    using namespace std;
//    if((index==1 || index==2) && ui->DBType->currentIndex()==0){
//        QMessageBox q;
//        q.information(this,"RoboJournal", "These settings are not applicable to SQLite databases. You should click OK instead");
//    }
}

void JournalCreator::on_buttonBox_accepted()
{
    CreateJournal();
}

void JournalCreator::on_buttonBox_rejected()
{
    // If this is the firstrun, return to the FirstRun class. It doesn't exist anymore at this
    //point, so create a new one. No one will ever know.
    if(Buffer::firstrun){
        this->reject();
        FirstRun f;
        f.exec();
    }
    else{
        this->reject();
    }
}
