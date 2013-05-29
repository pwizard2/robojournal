#include "mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/newjournalcreator.h"
#include <iostream>

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

// Gauge password strength (0-100) based on certain rules and return the value as an integer.
// --Will Kraft 5/29/13
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

         score=len*5; // multiply password length by five
         score=score+(numbers*4); // add numbers (multiplied by four)
         score=score+(symbols*6); // add number of symbols multiplied by 6
         score=score+(len-alphaUC)*2.5; // calculate uppercase letters.
         score=score+(len-alphaLC)*2.5; // calculate lowercase letters.

         QRegExp repeat_num("\\d\\d{1,}");

         int repeat_num_count=passwd.count(repeat_num);
         cout << "repeating numbers: " << repeat_num_count << endl;
         score=score-(repeat_num_count*4);

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

    if((ui->Password1->text() == ui->Password2->text()) && (ui->Password1->text().length() > 0)
            && (ui->Password1->text().length() >= 7)){
        ui->MatchNotify->setText("Passwords match");
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: green");

        // Calculate password strength
        int strength=PasswordStrength(ui->Password1->text());
        ui->StrengthMeter->setValue(strength);

        // Tell NewJournalCreator to unlock the OK button.
        emit unlockOK();
    }
    else
    {
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: red");

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
