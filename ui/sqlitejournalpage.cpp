#include "sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <iostream>
#include "core/buffer.h"

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



    if(Buffer::use_my_journals){
        QDir my_journals(QDir::homePath() + QDir::separator() + "Documents" + QDir::separator() +"My Journals");
        ui->DatabaseLocation->setText(my_journals.path());
    }
    else{
        QDir homefolder=QDir::homePath();
        ui->DatabaseLocation->setText(homefolder.path());
    }

    ui->AddToFavorites->setChecked(true);

    if(Buffer::use_my_journals){
        Create_My_Journals_Folder();
    }
}

void SQLiteJournalPage::Browse(QString startpath){

    QString folder=QFileDialog::getExistingDirectory(this,"Select Directory",startpath,QFileDialog::ShowDirsOnly);

    if(!folder.isEmpty()){
        ui->DatabaseLocation->setText(folder);
    }
    else{
        ui->DatabaseLocation->setText(startpath);
    }


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

// (6/5/13) Create the "My Journals" folder in users home dir if it does not already exist
void SQLiteJournalPage::Create_My_Journals_Folder(){

    using namespace std;
    QDir my_journals(QDir::homePath() + QDir::separator() + "Documents" + QDir::separator() +"My Journals");

    if(!my_journals.exists()){
        cout << "OUTPUT: Creating \"My Journals\" folder in home directory..." << endl;
        my_journals.mkdir(my_journals.path());
    }
    else{
        cout << "Detected \"My Journals\" folder in " + my_journals.path().toStdString() << endl;
    }
}

// (6/3/13) Process raw filename
void SQLiteJournalPage::ProcessFilename(QString filename, bool valid){

    int length=filename.length()-3;

    QRegExp extension(".db");

    // save possible regexp for filename: [a-zA-Z0-9_]*.db

    if((!filename.contains(extension)) && (!filename.isEmpty())  && (valid)){

        filename.append(".db");

        if(length != 0){
            filename=filename.toLower();
            ui->DatabaseName->setText(filename);
            ui->DatabaseName->setCursorPosition(ui->DatabaseName->text().size()-3);

            if(filename.length()-3==0){
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
