#include "journalcreatorcoverpage.h"
#include "ui_journalcreatorcoverpage.h"

JournalCreatorCoverPage::JournalCreatorCoverPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JournalCreatorCoverPage)
{
    ui->setupUi(this);
}

JournalCreatorCoverPage::~JournalCreatorCoverPage()
{
    delete ui;
}
