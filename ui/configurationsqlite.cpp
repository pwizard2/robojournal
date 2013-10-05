#include "configurationsqlite.h"
#include "ui_configurationsqlite.h"
#include "ui/newconfig.h"
#include "core/buffer.h"
#include "core/favoritecore.h"
#include <QMessageBox>
#include <QDir>

//###################################################################################################
ConfigurationSQLite::ConfigurationSQLite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationSQLite)
{
    ui->setupUi(this);
    PopulateForm();

    FavoriteCore f;

    // Check for new SQLite journals in the "My Journals folder (10/5/13).
    f.Auto_Populate_SQLite_Favorites();

    // Remove any items that no longer exist from the database (10/5/13).
    f.Do_Maintenance_SQLite();

    // Populate the UI with the list of known journals after all maintenance procedures are done.
    Show_Known_Journals();
}

//###################################################################################################
ConfigurationSQLite::~ConfigurationSQLite()
{
    delete ui;
}

//###################################################################################################
void ConfigurationSQLite::GetChanges(){

    Newconfig::new_use_my_journals=ui->UseMyJournals->isChecked();
    Newconfig::new_sqlite_default=ui->DefaultJournal->text();

    Harvest_Favorite_Databases();
}

//###################################################################################################
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

    // On windows, omit the part about the ~ home directory.
#ifdef _WIN32:
    ui->label_5->setText("<p>The following list contains all known SQLite journals that are associated with you:</p>");
    #endif
}

//###################################################################################################
// Get the list of known SQLite journals. New for 0.5. --Will Kraft, 9/21/13.
void ConfigurationSQLite::Show_Known_Journals(){

    FavoriteCore f;
    QIcon db(":/icons/database.png");

    QList<QStringList> journals=f.SQLite_getKnownJournals();

    for(int i=0; i<journals.size(); i++){

        QListWidgetItem *item=new QListWidgetItem(ui->Favorites);
        QStringList nextitem=journals.at(i);

        item->setText(nextitem.at(1));
        item->setToolTip("<p>Double-click this item to set <b>" + nextitem.at(1)+
                         "</b> as the default SQLite journal. Check this item's box to select it as a favorite.</p>");
        item->setIcon(db);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

        switch(nextitem.at(2).toInt()){
        case 0:
            item->setCheckState(Qt::Unchecked);
            break;

        case 1:
            item->setCheckState(Qt::Checked);
            break;
        }

        // apply special treatment to default db.
        if(nextitem.at(1)==Buffer::sqlite_default){
            ApplyDefaultProperties(item);
            default_db=item;
        }

        // If there is only one SQLite journal, automatically set it as default.
        if(journals.size()==1){
            ApplyDefaultProperties(item);
            default_db=item;
            ui->DefaultJournal->setText(nextitem.at(1));
        }
    }
}

//###################################################################################################
void ConfigurationSQLite::on_Favorites_itemDoubleClicked(QListWidgetItem *item)
{
    QString new_choice=item->text();
    QMessageBox m;

    if(new_choice != Buffer::sqlite_default){
        if(Buffer::showwarnings){


            int choice=m.question(this,"RoboJournal","Do you want to replace the current default journal with <b>"
                                  + new_choice +"</b>?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            switch(choice){

            case QMessageBox::Yes:
                ui->DefaultJournal->setText(new_choice);
                demoteDatabase(default_db);
                ApplyDefaultProperties(item);
                default_db=item;
                break;
            }
        }
    }
    else{
        m.critical(this,"RoboJournal","<b>" + new_choice + "</b> is already the default SQLite journal.");
    }
}

//###################################################################################################
// Apply special properties to the default database QtreeWidgetItem by bolding the text
// and giving it a gold database icon. Also, make sure it is checked.
// New for 0.5. --Will Kraft 7/19/13.
void ConfigurationSQLite::ApplyDefaultProperties(QListWidgetItem *item){

    QFont special;
    special.setBold(true);
    //special.setUnderline(true);
    item->setFont(special);

    item->setCheckState(Qt::Checked);
    QIcon gold_db(":/icons/database-gold.png");
    item->setIcon(gold_db);

    //Bugfix (7/31/13): prevent user from un-checking the box of the current default item.
    item->setFlags(Qt::ItemIsEnabled);

    Buffer::sqlite_default=item->text();
}

//###################################################################################################
// Demote the old default database whenever a new one is chosen from the list.
// New for 0.5. --Will Kraft (7/20/13)
void ConfigurationSQLite::demoteDatabase(QListWidgetItem *item){

    QFont normal;
    normal.setBold(false);

    item->setFont(normal);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

    item->setCheckState(Qt::Checked);
    QIcon db(":/icons/database.png");
    item->setIcon(db);
}

//###################################################################################################
// New for 0.5. Get the checked state of each journal in the Known Journals List (7/20/13). Adapted for
// SQLite on 9/21/13. This uses a "for" loop because there is no such thing as a QListWidgetItemIterator.
void ConfigurationSQLite::Harvest_Favorite_Databases(){

    FavoriteCore f;

    for(int i=0; i < ui->Favorites->count(); i++){

        QListWidgetItem *current=ui->Favorites->item(i);

        if(Qt::Checked == current->checkState()){
            f.SQLite_setFavoritebyName(current->text(),true);
        }
        else{
            f.SQLite_setFavoritebyName(current->text(),false);
        }
    }
}
