/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
    MADE IN USA

    RoboJournal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RoboJournal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RoboJournal.  If not, see <http://www.gnu.org/licenses/>.

    CLASS DESCRIPTION: This class is basically legacy code since a built-in tagger
    was added to the Editor class in version 0.5. However, this class still has
    its uses when teamed up with the TagReminder class (where launching the full
    Editor is impractical). In version 0.5 I gave this module a facelift to
    make it look like the EditorTagManager class to prevent user confusion.
    --Will Kraft, 6/14/13

  */

#include "ui/tagger.h"
#include "ui_tagger.h"
#include <QStringList>
#include <QListIterator>
#include <QMessageBox>
#include "sql/mysqlcore.h"
#include "core/buffer.h"
#include <QListWidgetItem>
#include <QIcon>
#include <iostream>
#include <QAbstractButton>
#include <QInputDialog>
#include "core/taggingshared.h"
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>
#include <QAbstractButton>
#include <QPushButton>

QString Tagger::id_num;



//#########################################################################################################

Tagger::Tagger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tagger)
{
    ui->setupUi(this);
    PrimaryConfig();


}

//#########################################################################################################

Tagger::~Tagger()
{
    delete ui;
}


//#########################################################################################################
// (6/14/13): New PrimaryConfig for 0.5 and later completely changes the appearance of the Tagger
// from previous versions.
void Tagger::PrimaryConfig(){

    QWidget* spacer1 = new QWidget();
    spacer1->setMinimumWidth(7);
    spacer1->setMaximumWidth(7);
    spacer1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer2 = new QWidget();
    spacer2->setMinimumWidth(7);
    spacer2->setMaximumWidth(7);
    spacer2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);


    QToolBar *bar = new QToolBar(this);
    bar->addWidget(ui->NewTag);
    bar->addSeparator();
    bar->addWidget(ui->RemoveTag);
    bar->addWidget(ui->AddTag);
    bar->addWidget(spacer2);
    bar->addWidget(ui->label);
    bar->addWidget(spacer1);
    bar->addWidget(ui->TagChooser);


    QVBoxLayout *layout=new QVBoxLayout(this);

    layout->addWidget(bar);
    layout->addWidget(ui->TagList);
    layout->addWidget(ui->line);
    layout->addWidget(ui->buttonBox);

    this->setLayout(layout);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    // Do initial setup
    ui->RemoveTag->setDisabled(true);

    QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setDisabled(true);

    // lock the Apply button because we haven't done anything yet
    QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
    apply->setDisabled(true);

    // set background and stylesheet for TagList element
    QPalette pal;
    QBrush bg=pal.light();
    QColor bgcolor=bg.color();
    ui->TagList->setStyleSheet("padding: 8px; background-color: "+ bgcolor.name() + ";");

    // Load Tags
    LoadTags();

    // Create Tag List
    TagAggregator();
    ui->TagChooser->clearEditText();

    // Add the fat spacer and the RevertTags button now because the layout has been applied
    // by this point. This means the spacer should stretch to fit the layout (docking the button on the right).
    QWidget* fat_spacer = new QWidget();
    fat_spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    bar->addWidget(fat_spacer);
    bar->addWidget(ui->RevertTags);
}

//#########################################################################################################
// Add a tag to the list

void Tagger::AddTag(QString newtag){
    using namespace std;

    // get rid of semicolons since that is how tags are delimited in the database
    newtag=newtag.replace(";","");

    bool add_entry=true;

    int count=ui->TagList->count();

    for(int i=0; i < count; i++){

        QListWidgetItem *c=ui->TagList->item(i);
        //cout << "Loop: " << i << endl;
        if(c->text()== newtag){
            add_entry=false;
            break;
        }
    }

    if(!add_entry){
        QMessageBox m;
        m.critical(this,"RoboJournal", "This entry has already been tagged with \""
                   + newtag + "\".");
        ui->TagChooser->clearEditText();
        ui->TagChooser->setFocus();

    }
    else{
        // Create new ListwidgetItem
        QIcon tagicon(":/icons/tag_orange.png");
        QListWidgetItem *entry=new QListWidgetItem(tagicon,newtag);
        ui->TagList->addItem(entry);
        ui->TagList->setCurrentItem(entry);
        ui->RemoveTag->setEnabled(true);
        ui->TagChooser->setFocus();
        ui->TagChooser->clearEditText();
        bool add_to_list=true;

        for(int a=0; a < ui->TagChooser->count(); a++){
            if(ui->TagChooser->itemText(a)==newtag){
                add_to_list=false;
                break;
            }
        }

        if(add_to_list){
            ui->TagChooser->addItem(newtag);

        }

        // Unlock the Apply button once we have changed tag data
        QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
        apply->setEnabled(true);

        // re-unlock Cancel button
        QAbstractButton *cancel=ui->buttonBox->button(QDialogButtonBox::Cancel);
        if(!cancel->isEnabled()){
            cancel->setEnabled(true);
        }
    }
}

//#########################################################################################################
// Add Tag to the list
void Tagger::AddTagToList(){

    // get the latest batch of tags just in case new ones have been added since the form was loaded
    QStringList current_list;
    for(int i=0; i < ui->TagChooser->count(); i++){
        current_list.append(ui->TagChooser->itemText(i));
    }

    TaggingShared ts;
    QString tag=ts.DefineTag(current_list);

        if(!tag.isEmpty()){
            QIcon newicon(":/icons/tag_red_add.png");
            ui->TagChooser->insertItem(0,newicon,tag);

            // if tag append button is disabled enable it now.
            if(!ui->AddTag->isEnabled()){
                ui->AddTag->setEnabled(true);

            }
        }

        ui->TagChooser->setEnabled(true);
        ui->TagChooser->setCurrentIndex(0);
}

//#########################################################################################################
// Create Tag Aggregator list. This populates the drop-down list with tags.

void Tagger::TagAggregator(){
    using namespace std;

    TaggingShared ts; //New shared tagging class for 0.5 (6/11/13)
    QStringList tag_list=ts.TagAggregator();

    QIcon tagicon(":/icons/tag_red.png");
    int count=0;

    for(int z=1; z < tag_list.size(); z++){
        QString text=tag_list[z];
        ui->TagChooser->addItem(tagicon,text);
        count++;
    }

    // bugfix 9/14/12: Prevent user from adding a blank tag to the entry. This can happen in brand-new journals with no tag data.
    // If there are no tags, disable the Add Tag button & Tag list.
    if(ui->TagChooser->count()==0){
        ui->AddTag->setDisabled(true);
        ui->TagChooser->setDisabled(true);
    }
}

//#########################################################################################################
// Delete a tag entry from the list

void Tagger::DeleteTag(){
    using namespace std;
    QListWidgetItem *item=ui->TagList->currentItem();
    delete item;

    // if there are no items left disable the Remove button.
    if(ui->TagList->count() == 0){
        ui->RemoveTag->setDisabled(1);
    }

    // Unlock the Apply button once we have changed tag data
    QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
    apply->setEnabled(true);

    // re-unlock Cancel button
    QAbstractButton *cancel=ui->buttonBox->button(QDialogButtonBox::Cancel);
    if(!cancel->isEnabled()){
        cancel->setEnabled(true);
    }
}

//#########################################################################################################
// return a semicolon-delimited QString containing all tags
QString Tagger::ExportTagList(){
    using namespace std;
    QStringList taglist;

    for(int i=0; i < ui->TagList->count(); i++){
        QListWidgetItem *current=ui->TagList->item(i);
        taglist.append(current->text());
    }

    QString tags=taglist.join(";");
    //cout << "Tags: " << tags.toStdString() << endl;
    return tags;
}

//#########################################################################################################
// Load tagdata from database and convert to tag objects in GUI

void Tagger::LoadTags(){
    using namespace std;
    TaggingShared ts;
    QStringList t_array=ts.FetchTags(Tagger::id_num);

    for(int i=0; i < t_array.size(); i++){

        // Exclude null entries from tag list
        if((t_array.at(i) != "null") && (t_array.at(i) != "Null")){
            QIcon tagicon(":/icons/tag_orange.png");
            QListWidgetItem *entry=new QListWidgetItem(tagicon,t_array.at(i));
            ui->TagList->addItem(entry);
        }
    }
}

//#########################################################################################################
// delete tag from a list
void Tagger::on_RemoveTag_clicked()
{
    DeleteTag();
}

//#########################################################################################################

void Tagger::on_TagList_itemSelectionChanged()
{
    ui->RemoveTag->setEnabled(true);
}

//#########################################################################################################
void Tagger::on_AddTag_clicked()
{
    QString newtag=ui->TagChooser->currentText();
    AddTag(newtag);
}

void Tagger::on_TagChooser_editTextChanged(const QString &arg1)
{
    if(arg1==""){
        ui->AddTag->setDisabled(true);
    }
    else{
        ui->AddTag->setEnabled(true);
    }
}



void Tagger::on_buttonBox_clicked(QAbstractButton *button)
{
    using namespace std;
    if(button==ui->buttonBox->button(QDialogButtonBox::Apply)){
        QString tag_data=ExportTagList();

        // unlock OK button
        QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
        ok->setEnabled(true);

        // lock the Apply button once we are finished with it
        QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
        apply->setDisabled(true);

        // Lock the Cancel button once we Apply b/c it would make no sense to cancel anymore
        QAbstractButton *cancel=ui->buttonBox->button(QDialogButtonBox::Cancel);
        cancel->setDisabled(true);


        if(Buffer::backend=="MySQL"){
            MySQLCore m;
            bool success=m.UpdateTags(tag_data,Tagger::id_num);

            if(success){
                cout << "OUTPUT: Tag data updated successfully" << endl;
            }
            else{
                cout << "ERROR: Tag data failed to update!" << endl;
            }
        }

    }
}

void Tagger::on_NewTag_clicked()
{
    AddTagToList();
}

void Tagger::on_TagChooser_currentIndexChanged()
{
    ui->AddTag->setEnabled(true);
}
