#include "mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/newjournalcreator.h"

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

void MySQLJournalPage::PrimaryConfig(){

    ui->MatchNotify->setStyleSheet("font-weight: bold; color: red");
    ui->JournalHost->setPlaceholderText("localhost");
    ui->Port->setPlaceholderText("3306");
    ui->StrengthMeter->setValue(0);
}

void MySQLJournalPage::ClearForm(){

}

// check to see if the passwords match and emit the correct signal
// back to the NewJournalCreator instance.
void MySQLJournalPage::PasswordsMatch(){

    if((ui->Password1->text() == ui->Password2->text()) && (ui->Password1->text().length() > 0)){
        ui->MatchNotify->setText("Passwords match");
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: green");
        emit unlockOK();
    }
    else{
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: red");

        if(ui->Password1->text().length()==0 && ui->Password2->text().length()==0){
            ui->MatchNotify->setText("Passwords are too short");
            emit unlockNotOK();
        }
        else{
            ui->MatchNotify->setText("Passwords do not match");
            emit unlockNotOK();
        }
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

