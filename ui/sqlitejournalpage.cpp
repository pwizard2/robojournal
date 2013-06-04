#include "sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

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

    QString folder=QFileDialog::getExistingDirectory(this,"Select Directory",startpath,QFileDialog::ShowDirsOnly);
    ui->DatabaseLocation->setText(folder);

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
void SQLiteJournalPage::ProcessFilename(QString filename, bool valid){

    int length=filename.length()-3;

    QRegExp extension(".db");


    if((!filename.contains(extension)) && (!filename.isEmpty())  && (valid)){

        filename.append(".db");

        if(length != 0){
            filename=filename.toLower();
            ui->DatabaseName->setText(filename);
            ui->DatabaseName->setCursorPosition(ui->DatabaseName->text().size()-3);

            if(filename.length()-3==0){
                ui->DatabaseName->clear();
                emit unlockNotOK();
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


void SQLiteJournalPage::on_DatabaseName_textChanged(const QString &arg1)
{
    QString filename=arg1;


    filename=filename.trimmed();
    filename=filename.toLower();


    bool valid=FilenameValid(filename);
    ProcessFilename(filename,valid);

}

void SQLiteJournalPage::ClearForm(){
    ui->DatabaseName->clear();
    PrimaryConfig();
}

void SQLiteJournalPage::on_BrowseButton_clicked()
{
    QString startpath=ui->DatabaseLocation->text();
    Browse(startpath);
}
