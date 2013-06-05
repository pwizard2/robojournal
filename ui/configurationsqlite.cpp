#include "configurationsqlite.h"
#include "ui_configurationsqlite.h"
#include "ui/newconfig.h"
#include "core/buffer.h"

ConfigurationSQLite::ConfigurationSQLite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationSQLite)
{
    ui->setupUi(this);
    PopulateForm();
}

ConfigurationSQLite::~ConfigurationSQLite()
{
    delete ui;
}

void ConfigurationSQLite::GetChanges(){

    Newconfig::new_use_my_journals=ui->UseMyJournals->isChecked();
    Newconfig::new_sqlite_default=ui->DefaultJournal->text();

    QStringList favorites;

    for(int i=0; i< ui->Favorites->count(); i++){
        QListWidgetItem *item=ui->Favorites->item(i);
        favorites.append(item->text());
    }

    Newconfig::new_sqlite_favorites=favorites;
}

void ConfigurationSQLite::PopulateForm(){

    ui->UseMyJournals->setChecked(Buffer::use_my_journals);

    if(Buffer::sqlite_default.isEmpty()){
        ui->DefaultJournal->setPlaceholderText("Warning: No default SQLite journal!");
    }
    else{
        ui->DefaultJournal->setText(Buffer::sqlite_default);

    }

    for(int a=0; a < Buffer::sqlite_favorites.size(); a++){
        QListWidgetItem *item=new QListWidgetItem();
        item->setText(Buffer::sqlite_favorites.at(a));
        ui->Favorites->addItem(item);
    }
}
