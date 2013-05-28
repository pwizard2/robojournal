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
    ui->StrengthMeter->setValue(0);
}

// check to see if the passwords match
bool MySQLJournalPage::PasswordsMatch(){

    if((ui->Password1->text() == ui->Password2->text()) && (ui->Password1->text().length() > 0)){
        ui->MatchNotify->setText("Passwords match");
        ui->MatchNotify->setStyleSheet("font-weight: bold; color: green");
        return true;
    }
    else{

        ui->MatchNotify->setStyleSheet("font-weight: bold; color: red");

        if(ui->Password1->text().length()==0 && ui->Password2->text().length()==0){
            ui->MatchNotify->setText("Passwords are too short");
            return false;
        }
        else{
            ui->MatchNotify->setText("Passwords do not match");
            return false;
        }
    }
}

void MySQLJournalPage::on_Password1_textChanged()
{
    bool unlock=PasswordsMatch();

}

void MySQLJournalPage::on_Password2_textEdited()
{
    bool unlock=PasswordsMatch();



}
