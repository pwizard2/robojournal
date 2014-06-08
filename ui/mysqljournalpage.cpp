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

    6/23/13: This class contains all journal creation functions for MySQL backends.
*/

#include "mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/newjournalcreator.h"
#include <iostream>
#include <QMessageBox>
#include "core/buffer.h"
#include "sql/sqlshield.h"
#include "core/favoritecore.h"
#include <QtNetwork/QNetworkInterface>

MySQLJournalPage::MySQLJournalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySQLJournalPage)
{
    ui->setupUi(this);
    PrimaryConfig();

    //Debug info for network, comment out when finished.
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int nIter=0; nIter<list.count(); nIter++)
    {
          qDebug() << list[nIter].toString();
    }

}

//#################################################################################################
MySQLJournalPage::~MySQLJournalPage()
{
    delete ui;
}

//#################################################################################################
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

    QString proposedName=sysuser + "_journal";

    //Check to see if the journal name is already in use (1/19/14)
    FavoriteCore f;
    bool isUnique=f.Check_For_Existing_Name(proposedName);

    if(!isUnique){
       //QString new_name=alternateName(proposedName);
      // ui->JournalName->setText(new_name);
        QMessageBox m;
        m.critical(this,"RoboJournal","The current journal name (<b>" + proposedName +
                   "</b>) is already in use. Please enter a different name.");
        ui->JournalName->clear();
        ui->JournalName->setFocus();
    }
    else{
        ui->JournalName->setText(proposedName);
    }

    ui->Username->setText(sysuser);
}

//#################################################################################################
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

//#################################################################################################
// Calculate password strength (0-100) based on certain rules and return the score as an integer.
//  --Will Kraft 5/29/13
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

        int total_deductions=0;

        // Dock points for adjacent lowercase letters
        QRegExp lowercase_adj("[a-z]{2,}",Qt::CaseSensitive);
        int repeat_lc=passwd.count(lowercase_adj);

        if(repeat_lc > 0){
            cout << "WARNING: Docking " << repeat_lc*len << " points from score because password has " << repeat_lc <<
                    " adjacent lowercase letters."  << endl;
            score=score-(repeat_lc*len);
            total_deductions=total_deductions+(repeat_lc*len);
        }


        // Dock points for adjacent uppercase letters
        QRegExp uppercase_adj("[A-Z]{2,}",Qt::CaseSensitive);
        int repeat_uc=passwd.count(uppercase_adj);

        if(repeat_uc > 0){
            cout << "WARNING: Docking " << repeat_uc*len << " points from score because password has " << repeat_uc <<
                    " adjacent uppercase letters."  << endl;
            score=score-(repeat_uc*len);
            total_deductions=total_deductions+(repeat_uc*len);
        }


        // Dock points for adjacent numbers
        QRegExp int_consecutive("[0-9]{2,}");
        int repeat_int=passwd.count(int_consecutive);

        if(repeat_int > 0){
            cout << "WARNING: Docking " << repeat_int*len << " points from score because password has " << repeat_int <<
                    " adjacent integers."  << endl;
            score=score-(repeat_int*len);
            total_deductions=total_deductions+(repeat_int*len);
        }

        // Dock points for repeated numbers (6/26/13)
        QString num_seed="1,2,3,4,5,6,7,8,9,0";
        QStringList num_array=num_seed.split(",");

        for(int i=0; i < num_array.size(); i++){
            QString var1=num_array.at(i);
            int num_dupes=passwd.count(var1);

            if(num_dupes >= 2){
                cout << "WARNING: Docking " << num_dupes * 4 << " points from score because password has " << num_dupes <<
                        " occurrences of " << var1.toStdString() << "." << endl;
                score=score-(num_dupes * 4);

                total_deductions=total_deductions+(num_dupes * 4);
            }
        }

        // Dock points for multiple occurrences of each UC/LC letter (6/26/13)
        QString alpha_seed="a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z"; // Now I know my ABC's...
        QStringList alpha_array=alpha_seed.split(",");

        for(int i=0; i < alpha_array.size(); i++){
            QString var2=alpha_array.at(i);
            int alpha_dupes=passwd.count(var2,Qt::CaseInsensitive);

            if(alpha_dupes >= 2){
                cout << "WARNING: Docking " << alpha_dupes * 4 << " points from score because password has " << alpha_dupes <<
                        " occurrences of " << var2.toStdString() << "." << endl;
                score=score-(alpha_dupes * 4);

                total_deductions=total_deductions+(alpha_dupes * 4);
            }
        }

        // Dock points for multiple occurrences of symbols. (6/23/13)
        QString sym_seed="!,@,#,$,%,^,&,*,(,),[,],{,},?,~,`,\",\'";
        QStringList sym_array=sym_seed.split(",");

        for(int i=0; i < sym_array.size(); i++){
            QString var3=sym_array.at(i);
            int sym_dupes=passwd.count(var3);

            if(sym_dupes >= 2){
                cout << "WARNING: Docking " << sym_dupes * 4 << " points from score because password has " << sym_dupes <<
                        " occurrences of " << var3.toStdString() << "." << endl;
                score=score-(sym_dupes * 4);

                total_deductions=total_deductions+(sym_dupes * 4);
            }
        }

        // prevent score from going out of range
        if(score>100){
            score=100;
        }

        if(score < 0){
            score=0;
        }

        // make sure the score is a whole integer w/o decimals
        score=qRound(score);

        cout << "================================================================================" << endl;
        cout << "Total Point Deductions: " << total_deductions << " / 100 " << endl;
    }

    return score;
}

//#################################################################################################
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

//#################################################################################################
void MySQLJournalPage::on_Password1_textChanged()
{
    PasswordsMatch();
}

//#################################################################################################
void MySQLJournalPage::on_Password2_textEdited()
{
    PasswordsMatch();
}

//#################################################################################################
// Make some adjustments after editing is finished if the user types a custom journal name
void MySQLJournalPage::on_JournalName_editingFinished()
{
    QString journal_name=ui->JournalName->text();
    journal_name=journal_name.trimmed();
    journal_name=journal_name.toLower();
    journal_name=journal_name.replace(" ","_");
    ui->JournalName->setText(journal_name);

}

//#################################################################################################
// Do the same thing for the username field
void MySQLJournalPage::on_Username_editingFinished()
{
    QString username=ui->Username->text();
    username=username.trimmed();
    username=username.toLower();
    username=username.replace(" ","_");
    ui->Username->setText(username);
}

//#################################################################################################
// Validate form data. This should be done before the form get submitted.
bool MySQLJournalPage::Validate(){

    QMessageBox a;
    QString journal=ui->JournalName->text();

    if(journal.isEmpty()){
        a.critical(this,"RoboJournal","You must provide a name for this journal!");
        ui->JournalName->setFocus();
        return false;
    }

    if(ui->Username->text().isEmpty()){
        a.critical(this,"RoboJournal","You must enter a username!");
        ui->Username->setFocus();
        return false;
    }

    // Check for existing journal names (1/19/14).
    FavoriteCore f;
    bool unique=f.Check_For_Existing_Name(journal);

    if(!unique){

        // Force the user to enter a different name to prevent duplicates. (--Will Kraft 4/6/14).
        QMessageBox m;
        m.critical(this,"RoboJournal","The current journal name (<b>" + journal +
                   "</b>) is already in use. Please enter a different name.");
        ui->JournalName->clear();
        ui->JournalName->setFocus();

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
                              "Do you really want to use it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

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

//#################################################################################################
// Get raw form data from widgets, sanitize it, and shunt it back to the NewJournalCreator class.
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

//#################################################################################################
// Create an alternate name (i.e. journal_name2 if journal_name already exists) --Will Kraft (1/19/14).
QString MySQLJournalPage::alternateName(QString proposedName){

    QRegExp digit("\\d+");
    QString new_name;

    if(proposedName.lastIndexOf(digit)==proposedName.length()){
        int pos = digit.indexIn(proposedName);
        if (pos > -1){
            int num = digit.cap(1).toInt();
            proposedName.chop(1);
            num++;
            new_name=proposedName + QString::number(num);
        }
    }
    else{
        proposedName.chop(1);
        new_name=proposedName + QString::number(2);
    }


    return new_name;
}
