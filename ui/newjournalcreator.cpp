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

    ui->PageArea->setWidget(stack);
    ui->PageArea->setWidgetResizable(true);
    ui->PageArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PageArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    stack->setCurrentIndex(0);
    ui->DatabaseType->setCurrentRow(0);


    if(Buffer::firstrun){
        ui->SetAsDefault->setChecked(true);
        ui->SetAsDefault->setEnabled(false);
    }

    connect(m, SIGNAL(unlockOK()), this, SLOT(unlockOKButton()));
    connect(m, SIGNAL(unlockNotOK()), this, SLOT(lockOKButton()));

}

void NewJournalCreator::on_DatabaseType_currentRowChanged(int currentRow)
{
    stack->setCurrentIndex(currentRow);
}

// Unlock the OK button to allow the user to submit the form. This also locks the
// DatabaseType object to prevent the user from switching journal types before clicking OK.
void NewJournalCreator::unlockOKButton(){

        QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(true);

        ui->DatabaseType->setEnabled(false);
}

// Lock the OK button to allow the user to prevent the form from being submitted  This also unlocks the
// DatabaseType object so the user can switch the journal type if necessary.
void NewJournalCreator::lockOKButton(){
        QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
        okButton->setEnabled(false); 
         ui->DatabaseType->setEnabled(true);
}
