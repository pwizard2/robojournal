#include "sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"

SQLiteJournalPage::SQLiteJournalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SQLiteJournalPage)
{
    ui->setupUi(this);
}

SQLiteJournalPage::~SQLiteJournalPage()
{
    delete ui;
}
