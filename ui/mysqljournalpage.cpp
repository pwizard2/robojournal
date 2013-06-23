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

    6/23/13: This class contains all journal creation functions for MySQL backends.
*/

#include "mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/newjournalcreator.h"
#include <iostream>
#include <QMessageBox>
#include "core/buffer.h"
#include "sql/sqlshield.h"

MySQLJournalPage::MySQLJournalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySQLJournalPage)
{
    ui->setupUi(this);
    PrimaryConfig();
}



MySQLJournalPage::~MySQLJournalPage()
{
    delete ui;
}

// 5/28/13: Setup the form with the correct default values immediately after instantiation. This is
// also called when the user clicks the "Restore Defaults" button in the NewJournalCreator class.
void MySQLJournalPage::PrimaryConfig(){

    ui->MatchNotify->setStyleSheet("font-weight: bold; color: red");
    ui->JournalHost->setPlaceholderText("localhost");
    ui->Port->setPlaceholderText("3306");
    ui->StrengthMeter->setValue(0);


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
}


// 5/28/13: Clear the MySQL form and restores default values to all applicable fields.
// This method is not invoked directly; it is called from a slot when the user clicks
// "Resotore Defaults" on the NewJournalCreator window.
void MySQLJournalPage::ClearForm(){

    ui->JournalHost->clear();
    ui->RootPassword->clear();
    ui->Port->clear();
    ui->Password1->clear();
    ui->Password2->clear();
    ui->Username->clear();
    ui->JournalName->clear();

    PrimaryConfig();
}

// Gauge password strength (0-100) based on certain rules and return the score as an integer.
// This doesn't take everything into account (yet). --Will Kraft 5/29/13
int MySQLJournalPage::PasswordStrength(QString passwd){
    using namespace std;

    int score=0;

    int len=passwd.length();

    // count of each element in passwd string
    int alphaUC=passwd.count("[A-Z]");
    int alphaLC=passwd.count("[a-z]");
    int numbers=passwd.count("[0-9");
    int symbols=passwd.count("!@#$%^&*()[]{}?~`\"\"");

    // start the logic
    if(!passwd.isEmpty()){

        score=len*5; // multiply password length by five and start the score with that value
        score=score+(numbers*4); // add number diversity bonus (multiplied by four)
        score=score+(symbols*6); // add symbol diversity bonus (multiplied by 6)
        score=score+(len-alphaUC)*2.5; // calculate uppercase letters.
        score=score+(len-alphaLC)*2.5; // calculate lowercase letters.

        // Dock points for adjacent lowercase letters
        QRegExp lowercase_consecutive("[a-z]{2,}",Qt::CaseSensitive);
        int repeat_lc=passwd.count(lowercase_consecutive);
        score=score-(repeat_lc*len);

        // Dock points for adjacent uppercase letters
        QRegExp uppercase_consecutive("[A-Z]{2,}",Qt::CaseSensitive);
        int repeat_uc=passwd.count(uppercase_consecutive);
        score=score-(repeat_uc*len);

        // Dock points for adjacent numbers
        QRegExp int_consecutive("[0-9]{2,}");
        int repeat_int=passwd.count(int_consecutive);
        score=score-(repeat_int*len);

        // prevent score from going out of range
        if(score>100){
            score=100;
        }

        // make sure the number is whole w/o decimals
        score=qRound(score);
    }

    return score;
}

// 5/28/13: Check to see if the passwords match and emit the correct signal
// back to the appropriate slots in the NewJournalCreator instance.
void MySQLJournalPage::PasswordsMatch(){

    QPixmap good(":/icons/tick-circle.png");
    QPixmap bad(":/icons/cross-circle.png");

    if((ui->Password1->text() == ui->Password2->text()) && (ui->Password1->text().length() > 0)
       && (ui->Password1->text().length() >= 7)){
        ui->MatchNotify->setText("Passwords match");
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: green");
        ui->MatchIcon->setPixmap(good);

        // Calculate password strength
        int strength=PasswordStrength(ui->Password1->text());
        ui->StrengthMeter->setValue(strength);

        // Tell NewJournalCreator to unlock the OK button.
        emit unlockOK();
    }
    else
    {
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: red");
        ui->MatchIcon->setPixmap(bad);

        if(ui->Password1->text().length()< 7 && ui->Password2->text().length() < 7){
            ui->MatchNotify->setText("Passwords are too short");
            emit unlockNotOK();
        }
        else{
            ui->MatchNotify->setText("Passwords do not match");

            // Tell NewJournalCreator to leave the OK button locked or re-lock it.
            emit unlockNotOK();
        }

        // clear strength meter
        ui->StrengthMeter->setValue(0);
    }
}


void MySQLJournalPage::on_Password1_textChanged()
{
    PasswordsMatch();
}

void MySQLJournalPage::on_Password2_textEdited()
{
    PasswordsMatch();
}

// Make some adjustments after editing is finished if the user types a custom journal name
void MySQLJournalPage::on_JournalName_editingFinished()
{
    QString journal_name=ui->JournalName->text();
    journal_name=journal_name.trimmed();
    journal_name=journal_name.toLower();
    journal_name=journal_name.replace(" ","_");
    ui->JournalName->setText(journal_name);

}

// Do the same thing for the username field
void MySQLJournalPage::on_Username_editingFinished()
{
    QString username=ui->Username->text();
    username=username.trimmed();
    username=username.toLower();
    username=username.replace(" ","_");
    ui->Username->setText(username);
}

// Validate form data. This should be done before the form get submitted.
bool MySQLJournalPage::Validate(){

    QMessageBox a;

    if(ui->JournalName->text().isEmpty()){
        a.critical(this,"RoboJournal","You must provide a name for this journal!");
        ui->JournalName->setFocus();
        return false;
    }

    if(ui->Username->text().isEmpty()){
        a.critical(this,"RoboJournal","You must enter a username!");
        ui->Username->setFocus();
        return false;
    }

    QRegExp root("root",Qt::CaseInsensitive);

    if(root.exactMatch(ui->Username->text())){
        a.critical(this,"RoboJournal","You cannot use <b>root</b> as a username!");
        ui->Username->setFocus();
        ui->Username->clear();
        return false;
    }

    if(ui->RootPassword->text().isEmpty()){

        QString hostname=ui->JournalHost->text();

        if(hostname.isEmpty()){
            hostname=ui->JournalHost->placeholderText();
        }

        a.critical(this,"RoboJournal","You must enter the MySQL root password for <b>" + hostname +"</b>.");
        ui->RootPassword->setFocus();
        return false;
    }

    if((ui->StrengthMeter->value() <= 50) && (Buffer::showwarnings)){
        QMessageBox m;
        int choice=m.question(this,"RoboJournal","The password you entered is weak and could be easily broken. "
                              "Are you sure you want to use it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        switch(choice){
            case QMessageBox::Yes:
            return true;
            break;

            case QMessageBox::No:
            return false;
            break;
        }
    }

    return true;
}

// Get form data from widgets and shunt it back to the NewJournalCreator class.
void MySQLJournalPage::HarvestData(){

    QString raw_host, raw_pass, raw_user, raw_root_pass, raw_port, raw_journal;

    if(ui->JournalHost->text().isEmpty()){
        raw_host=ui->JournalHost->placeholderText();
    }
    else{
        raw_host=ui->JournalHost->text();
    }

    raw_pass=ui->Password1->text();
    raw_user=ui->Username->text();
    raw_journal=ui->JournalName->text();
    raw_root_pass=ui->RootPassword->text();

    if(ui->Port->text().isEmpty()){
        raw_port=ui->Port->placeholderText();
    }
    else{
        raw_port=ui->Port->text();
    }

    // Break potential SQL injections so an attacker won't be able to nuke the database.
    // 0.5 Bugfix -- Will Kraft, 6/23/13

    SQLShield s;

    NewJournalCreator::hostname=s.Break_Injections(raw_host);
    NewJournalCreator::password=s.Break_Injections(raw_pass);
    NewJournalCreator::username=s.Break_Injections(raw_user);
    NewJournalCreator::journal_name=s.Break_Injections(raw_journal);
    NewJournalCreator::root_password=s.Break_Injections(raw_root_pass);
    NewJournalCreator::port=s.Break_Injections(raw_port);
}
