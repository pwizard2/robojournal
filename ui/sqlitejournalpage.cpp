#include "sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <iostream>
#include "core/buffer.h"
#include "ui/newjournalcreator.h"

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

    QString filename;

    if(Buffer::use_my_journals){
        QDir my_journals(QDir::homePath() + QDir::separator() + "Documents" + QDir::separator() +"My Journals");
        filename=my_journals.path();
        filename=QDir::toNativeSeparators(filename);

        ui->DatabaseLocation->setText(filename);
    }
    else{
        QDir homefolder=QDir::homePath();
        filename=homefolder.path();
        filename=QDir::toNativeSeparators(filename);
        ui->DatabaseLocation->setText(filename);
    }

    ui->AddToFavorites->setChecked(true);

    if(Buffer::use_my_journals){
        Create_My_Journals_Folder();
    }
}


// (6/6/13) Get data, validate/repair it, and return it to NewJournalCreator class.
bool SQLiteJournalPage::HarvestData(){
    using namespace std;

    NewJournalCreator::sqlite_journal_path=ui->DatabaseLocation->text();

    QRegExp test("[a-z|A-Z|0-9|_]*.db");

    if(test.exactMatch(ui->DatabaseName->text())){
        NewJournalCreator::sqlite_journal_name=ui->DatabaseName->text();
        return true;
    }
    else{
        QMessageBox m;
        m.critical(this,"RoboJournal","Journal name is invalid. RoboJournal will attempt to fix it for you.");

        // Attempt to fix the filename automatically
        QString filename=ui->DatabaseName->text();
        NewJournalCreator::sqlite_journal_name=filename;

        // Give 3 tries to fix the filename before returning false
        for(int i=0; i<3; i++){
            filename=filename.simplified();
            filename=filename.replace(" ","_");
            QRegExp bad_extension(".db.+");

            if(filename.contains(bad_extension)){
                filename=filename.remove(bad_extension);
                filename=filename.append(".db");
            }

            ui->DatabaseName->setText(filename);
            NewJournalCreator::sqlite_journal_name=filename;

            if(test.exactMatch(filename)){
                cout << "OUTPUT: Fixed errors in file name: " << filename.toStdString() << endl;
                break;
                return true;
            }
        }

        if(!test.exactMatch(filename)){
            m.critical(this,"RoboJournal","Journal name is still invalid. Please fix the filename manually (filename "
                       "must end with <b>.db</b> and contain no spaces).");
            return false;
        }
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
        cout << "OUTPUT: Detected \"My Journals\" folder in " + my_journals.path().toStdString() << endl;
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

