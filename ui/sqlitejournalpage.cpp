#include "sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"
#include <QDir>
#include <QFileDialog>

SQLiteJournalPage::SQLiteJournalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SQLiteJournalPage)
{
    ui->setupUi(this);
    PrimaryConfig();
}

SQLiteJournalPage::~SQLiteJournalPage()
{
    delete ui;
}

void SQLiteJournalPage::PrimaryConfig(){

    QDir homefolder=QDir::homePath();
    ui->DatabaseLocation->setText(homefolder.path());
    ui->AddToFavorites->setChecked(true);
}

void SQLiteJournalPage::Browse(QString startpath){

}

// (6/3/13) Validation function for filename
bool SQLiteJournalPage::FilenameValid(QString filename){

    if(filename.isEmpty()){
        return false;
    }

    QRegExp spaces("\\s+");
    if(spaces.exactMatch(filename)){
        return false;
    }

    return true;
}

// (6/3/13) Process raw filename
QString SQLiteJournalPage::ProcessFilename(QString raw){


}


void SQLiteJournalPage::on_DatabaseName_textChanged(const QString &arg1)
{
    QString filename=arg1;

    filename=filename.replace(" ","_");

    int length=filename.length()-3;
    filename=filename.trimmed();
    filename=filename.toLower();


    bool valid=FilenameValid(filename);

    QRegExp extension(".db");

    if((!filename.contains(extension)) && (!filename.isEmpty())  && (valid)){

        filename.append(".db");

        if(length != 0){
            filename=filename.replace("\\s","_");
                filename=filename.toLower();

            ui->DatabaseName->setText(filename);
            ui->DatabaseName->setCursorPosition(ui->DatabaseName->text().size()-3);

            if(filename==".db"){
                ui->DatabaseName->clear();
            }

            if(filename.count(extension) > 1){
                filename.truncate(filename.length()-3);
                ui->DatabaseName->setText(filename);
                ui->DatabaseName->setCursorPosition(ui->DatabaseName->text().size()-3);
            }
        }
        else{
            ui->DatabaseName->clear();
        }
    }



    if(valid){
        emit unlockOK();
    }
    else{
        emit unlockNotOK();
    }
}

void SQLiteJournalPage::ClearForm(){
    ui->DatabaseName->clear();
    PrimaryConfig();
}
