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
}

void SQLiteJournalPage::Browse(QString startpath){

}
