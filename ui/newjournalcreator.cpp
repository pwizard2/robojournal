#include "newjournalcreator.h"
#include "ui_newjournalcreator.h"
#include <QStackedWidget>
#include "ui/journalcreatorcoverpage.h"
#include "ui_journalcreatorcoverpage.h"
#include "ui/mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"
#include <QPushButton>
#include "core/buffer.h"

NewJournalCreator::NewJournalCreator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewJournalCreator)
{
    ui->setupUi(this);
    PrimaryConfig();
}

NewJournalCreator::~NewJournalCreator()
{
    delete ui;
}


void NewJournalCreator::PrimaryConfig(){

    QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Create stacked widget
    stack=new QStackedWidget(this);

    stack->addWidget(c=new JournalCreatorCoverPage(this));
    stack->addWidget(s=new SQLiteJournalPage(this));
    stack->addWidget(m=new MySQLJournalPage(this));
    stack->setCurrentIndex(2);


    ui->PageArea->setWidget(stack);
    ui->PageArea->setWidgetResizable(true);
    ui->PageArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PageArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    if(Buffer::firstrun){
        ui->SetAsDefault->setChecked(true);
        ui->SetAsDefault->setEnabled(false);
    }

}

void NewJournalCreator::on_DatabaseType_currentRowChanged(int currentRow)
{
    stack->setCurrentIndex(currentRow);
}

void NewJournalCreator::ManageOKButton(bool unlock){
    if(unlock){

        QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(true);
    }
    else{
        QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(false);
    }
}
