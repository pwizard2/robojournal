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
#include "sql/mysqlcore.h"
#include "sql/sqlitecore.h"

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

    QListWidgetItem *coversheet= ui->DatabaseType->item(0);
    QListWidgetItem *sqlite_db= ui->DatabaseType->item(1);
    QListWidgetItem *mysql_db= ui->DatabaseType->item(2);

    coversheet->setToolTip("<p>Display the cover sheet/introduction page.</p>");
    sqlite_db->setToolTip("<p>Create an empty SQLite-based journal.</p>");
    mysql_db->setToolTip("<p>Create an empty MySQL-based journal.</p>");


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


    if(Buffer::firstrun){
        ui->SetAsDefault->setChecked(true);
        ui->SetAsDefault->setEnabled(false);
    }

    connect(m, SIGNAL(unlockOK()), this, SLOT(unlockOKButton()));
    connect(m, SIGNAL(unlockNotOK()), this, SLOT(lockOKButton()));
    connect(this, SIGNAL(Clear_MySQL()), m, SLOT(ClearForm()));



    stack->setCurrentIndex(0);
    ui->DatabaseType->setCurrentRow(0);

}


// This method gets calkled whenever the user clicks the "Restore Defaults"
// button. This sends a signal to a slot on a specific page depending on
// which item is selected in the DatabaseType list.
void NewJournalCreator::RestoreDefaults(){

    switch(ui->DatabaseType->currentRow()){
    case 0:
        // Do nothing; This never gets called because the Restore Defaults
        // button is disabled while row 0 is selected in DatabaseType.
        break;

    case 1:
        // sqlite reset
        break;


    case 2: // MySQL reset
        emit Clear_MySQL();
        break;
    }
}

void NewJournalCreator::on_DatabaseType_currentRowChanged(int currentRow)
{
    stack->setCurrentIndex(currentRow);

    QPushButton *resetbutton=ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);

    if(currentRow==0){
        resetbutton->setEnabled(false);
    }
    else{
        resetbutton->setEnabled(true);
    }
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

void NewJournalCreator::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()=="Restore Defaults"){
        RestoreDefaults();
    }
}



void NewJournalCreator::on_BrowseButton_clicked()
{

}
