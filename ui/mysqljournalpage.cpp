#include "mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/newjournalcreator.h"
#include <iostream>
#include <QMessageBox>


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

    return true;
}

// Get form data from widgets and shunt it back to the NewJournalCreator class.
void MySQLJournalPage::HarvestData(){

    if(ui->JournalHost->text().isEmpty()){
        NewJournalCreator::hostname=ui->JournalHost->placeholderText();
    }
    else{
        NewJournalCreator::hostname=ui->JournalHost->text();
    }

    NewJournalCreator::password=ui->Password1->text();
    NewJournalCreator::username=ui->Username->text();
    NewJournalCreator::journal_name=ui->JournalName->text();
    NewJournalCreator::root_password=ui->RootPassword->text();

    if(ui->Port->text().isEmpty()){
        NewJournalCreator::port=ui->Port->placeholderText();
    }
    else{
        NewJournalCreator::port=ui->Port->text();
    }
}
