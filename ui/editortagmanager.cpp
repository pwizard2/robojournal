/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
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

    6/10/13: This class is the built-in tag interface for the Editor window.
    As of 7/12/13, it has become the universal tag interface for the entire
    application. The Tagger class used to contain a redundant copy of this
    code because it was the original tag management interface. Now, The
    Tagger is just a window frame that contains an instance of this object.
*/

#include "editortagmanager.h"
#include "ui_editortagmanager.h"
#include <QToolBar>
#include <QVBoxLayout>
#include "core/taggingshared.h"
#include <QIcon>
#include <QPalette>
#include <QBrush>
#include <QColor>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "ui/editor.h"
#include "core/buffer.h"
#include <iostream>
#include "ui/tagger.h"

EditorTagManager::EditorTagManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditorTagManager)
{
    ui->setupUi(this);
    PrimaryConfig();
}

int EditorTagManager::tag_count;
bool EditorTagManager::standalone_tagger;

// ###################################################################################################

EditorTagManager::~EditorTagManager()
{
    delete ui;
}

// ###################################################################################################

// Public method that retuns the contents of ui->TagList as a semicolon-separated QString.
// This method is NOT to be confused with the private function that gets tags from the database.
// The code for this was lifted directly from the old Tagger class. 6/29/13
QString EditorTagManager::HarvestTags(){
    using namespace std;

    QStringList taglist;

    for(int i=0; i < ui->TagList->count(); i++){
        QListWidgetItem *current=ui->TagList->item(i);
        taglist.append(current->text());
    }

    QString tags=taglist.join(";");

    //Output used for debugging purposes, comment out in final build
    //cout << "Tags: " << tags.toStdString() << endl;

    return tags;
}

// ###################################################################################################
// 6/14/13: Revert tags to their default values. This method is only accessible if the Editor
// is in Edit Mode (since new entries have no pre-existing data to revert to).
void EditorTagManager::RevertTags(){

    if(Buffer::showwarnings){
        QMessageBox m;
        int choice=m.question(this,"RoboJournal","Do you really want to discard the changes you made to this entry\'s tags?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(choice==QMessageBox::Yes){

            if(no_tags){
                ui->TagList->clear();
            }
            else{
                ui->TagList->clear();

                if(EditorTagManager::standalone_tagger){
                    emit Sig_UnlockTaggerApplyButton();

                    LoadTags(Tagger::id_num);
                }
                else{
                    LoadTags(Buffer::editentry);
                }
            }
        }
    }
    else{
        if(no_tags){
            ui->TagList->clear();
        }
        else{
            ui->TagList->clear();

            if(EditorTagManager::standalone_tagger){
                emit Sig_UnlockTaggerApplyButton();
                LoadTags(Tagger::id_num);
            }
            else{
                LoadTags(Buffer::editentry);
            }
        }
    }
}

// ###################################################################################################

// 6/13/13: Add a new tag to the AvailableTags list. This uses the new TaggingShared class.
void EditorTagManager::DefineTag(){

    // get the latest batch of tags just in case new ones have been added since the form was loaded
    QStringList current_list;
    for(int i=0; i < ui->AvailableTags->count(); i++){
        current_list.append(ui->AvailableTags->itemText(i));
    }

    TaggingShared ts;
    QString tag=ts.DefineTag(current_list);

    if(!tag.isEmpty()){
        QIcon newicon(":/icons/tag_red_add.png");
        ui->AvailableTags->insertItem(0,newicon,tag);
        ui->AvailableTags->setCurrentIndex(0);

        if(!ui->AddTag->isEnabled()){
            ui->AddTag->setEnabled(true);
        }
    }
}

// ###################################################################################################

// 6/14/13: Update current tag count in toolbar.
void EditorTagManager::TagCount(int count){

    QString taglabel;
    if(count==1){
        taglabel=" tag";
    }
    else{
        taglabel=" tags";
    }
    ui->TagCount->setText(QString::number(count) + taglabel);
}

// ###################################################################################################

// 6/10/13: Create toolbar and layout for this class.
void EditorTagManager::PrimaryConfig(){

    QWidget* spacer1 = new QWidget();
    spacer1->setMinimumWidth(7);
    spacer1->setMaximumWidth(7);
    spacer1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer2 = new QWidget();
    spacer2->setMinimumWidth(15);
    spacer2->setMaximumWidth(15);
    spacer2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer3 = new QWidget();
    spacer3->setMinimumWidth(7);
    spacer3->setMaximumWidth(7);
    spacer3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);


    // set background and stylesheet for TagList element
    QPalette pal;
    QBrush bg=pal.light();
    QColor bgcolor=bg.color();
    //ui->TagList->setStyleSheet("padding: 8px; background-color: "+ bgcolor.name() + ";");
    //ui->TagList->setStyleSheet("margin: 8px;");
    ui->TagCount->setText("0 tags");

    // Create toolbar.
    QToolBar *bar = new QToolBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    bar->setLayoutDirection(Qt::LeftToRight);
    bar->setContextMenuPolicy(Qt::DefaultContextMenu);

    QFont toolbarFont("Sans",7);
    bar->setFont(toolbarFont);

    if(Buffer::show_icon_labels){
        ui->NewTag->setText("Define Tag");
        ui->RemoveTag->setText("Remove Current Tag");
        ui->AddTag->setText("Add Current Tag");
        ui->RevertTags->setText("Revert Tags");

        ui->NewTag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->RemoveTag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->AddTag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->RevertTags->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        if(!standalone_tagger){
             setMaximumHeight(175);
        }
    }

    // Use full-size font for certain objects no matter what
    QFont fullFont("Sans",9);
    ui->TagCount->setFont(fullFont);
    ui->AvailableTags->setFont(fullFont);
    ui->SelectTag->setFont(fullFont);


    // Populate toolbar with loose UI elements.
    bar->addWidget(ui->NewTag);
    bar->addWidget(ui->RemoveTag);
    bar->addSeparator();
    bar->addWidget(ui->AddTag);
    bar->addWidget(spacer1);
    bar->addWidget(ui->SelectTag);
    bar->addWidget(spacer3);
    bar->addWidget(ui->AvailableTags);
    bar->addWidget(spacer2);
    bar->addWidget(ui->TagCount);

    //tighten up toolbar spacing for 0.5 (7/15/13).
    QSize barSize(16,16);
    bar->setIconSize(barSize);
    bar->setContentsMargins(0,2,0,0);


    // Force everything into a vboxlayout so the tag interface stretches to fill all available space.
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(bar,0);
    layout->addWidget(ui->TagList,0);


    this->setLayout(layout);

    QIcon inserticon(":/icons/tag--arrow.png");
    QIcon removeicon(":/icons/tag--minus.png");
    QIcon declareicon(":/icons/tag--pencil.png");
    QIcon reverticon(":/icons/arrow_rotate_clockwise.png");

    // Setup Context Menu for tagarea
    QAction *insert = new QAction(tr("Apply current tag"),this);
    insert->setIcon(inserticon);
    connect(insert, SIGNAL(triggered()), this, SLOT(s_addTag()));

    remove = new QAction(tr("Remove selected tag"),this);
    remove->setIcon(removeicon);
    connect(remove, SIGNAL(triggered()), this, SLOT(s_removeTag()));

    QAction *define = new QAction(tr("Define new tag"),this);
    define->setIcon(declareicon);
    connect(define, SIGNAL(triggered()), this, SLOT(s_newTag()));

    revert = new QAction(tr("Revert tag changes"),this);
    revert->setIcon(reverticon);
    connect(revert, SIGNAL(triggered()), this, SLOT(s_revertTag()));

    contextmenu=new QMenu(ui->TagList);
    contextmenu->addAction(define);
    contextmenu->addSeparator();
    contextmenu->addAction(insert);
    contextmenu->addAction(remove);
    contextmenu->addSeparator();
    contextmenu->addAction(revert);

    connect(ui->TagList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showPopup()));

    // Get list of tags and disable AddTag button if there are none

    CreateTagList();

    if(ui->AvailableTags->count()==0){
        ui->AddTag->setEnabled(false);

    }

    // if there are no items left disable the Remove button.
    if(ui->TagList->count() == 0){
        ui->RemoveTag->setDisabled(true);
        remove->setDisabled(true);

    }

    // Add the fat spacer and the RevertTags button now because the layout has been applied
    // by this point. This means the spacer should stretch to fit the layout (docking the button on the right).
    QWidget* fat_spacer = new QWidget();
    fat_spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    bar->addWidget(fat_spacer);
    bar->addWidget(ui->RevertTags);

    // Disable Revert Tags button if this is a new entry because a new item has no tags to revert to.
    if((!Buffer::editmode) && (standalone_tagger)){
        ui->RevertTags->setDisabled(true);
    }

}

// ###################################################################################################
//6/11/13: Create drop-down tag list. Use the new TaggingShared class.

void EditorTagManager::CreateTagList(){

    TaggingShared ts;
    QStringList tags=ts.TagAggregator();
    QIcon tagicon(":/icons/tag_red.png");
    int count=0;

    for(int z=1; z < tags.size(); z++){
        QString text=tags[z];
        ui->AvailableTags->addItem(tagicon,text);
        count++;
    }
}

// ###################################################################################################
// (6/11/13) Add currently-selected tag in ui > AvailableTags to tag list.
void EditorTagManager::AddTag(QString newtag){
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
        ui->AvailableTags->clearEditText();
        ui->AvailableTags->setFocus();

    }
    else{
        // Create new ListwidgetItem
        QIcon tagicon(":/icons/tag_orange.png");
        QListWidgetItem *entry=new QListWidgetItem(tagicon,newtag);
        ui->TagList->addItem(entry);
        ui->TagList->setCurrentItem(entry);
        ui->RemoveTag->setEnabled(true);
        this->remove->setEnabled(true);

        ui->AvailableTags->setFocus();
        ui->AvailableTags->clearEditText();
        bool add_to_list=true;

        for(int a=0; a < ui->AvailableTags->count(); a++){
            if(ui->AvailableTags->itemText(a)==newtag){
                add_to_list=false;
                break;
            }
        }

        if(add_to_list){
            ui->AvailableTags->addItem(newtag);
        }

        TagCount(ui->TagList->count());

        if(EditorTagManager::standalone_tagger){
            emit Sig_UnlockTaggerApplyButton();
        }
    }
}

// ###################################################################################################

// (6/11/13) Remove currently-selected tag from Tag list
void EditorTagManager::RemoveTag(){
    QListWidgetItem *item=ui->TagList->currentItem();
    delete item;

    // if there are no items left, disable the Remove button and context menu item.
    if(ui->TagList->count() == 0){
        ui->RemoveTag->setDisabled(true);
        remove->setDisabled(true);
    }

    TagCount(ui->TagList->count());

    if(EditorTagManager::standalone_tagger){
        emit Sig_UnlockTaggerApplyButton();
    }
}

// ###################################################################################################

// (6/14/13) Fetch current tags from database. This is NOT to be confused with the
// public method that Exports the tags.
void EditorTagManager::LoadTags(QString id){

    TaggingShared ts;
    QStringList tags=ts.FetchTags(id);

    // Remember if this entry originally had tags or not
    if(tags.size()==0){
        no_tags=true;
    }
    else{
        no_tags=false;
    }

    for(int i=0; i < tags.size(); i++){

        // Exclude null entries from tag list
        if((tags.at(i) != "null") && (tags.at(i) != "Null")){
            QIcon tagicon(":/icons/tag_orange.png");
            QListWidgetItem *entry=new QListWidgetItem(tagicon,tags.at(i));
            ui->TagList->addItem(entry);
        }
    }

    TagCount(ui->TagList->count());
}

// ###################################################################################################

// Slot that gets triggered when the user clicks the AddTag toolbar button.
void EditorTagManager::on_AddTag_clicked()
{
    QString newtag=ui->AvailableTags->currentText();
    AddTag(newtag);
}

// ###################################################################################################

// 6/13/13: slot that gets triggered from context menu -- add tag
void EditorTagManager::s_addTag(){

    QString newtag=ui->AvailableTags->currentText();
    AddTag(newtag);
}

// ###################################################################################################

// 6/13/13: slot that gets triggered from context menu -- remove selected tag
void EditorTagManager::s_removeTag(){
    RemoveTag();
}

// ###################################################################################################

// 6/13/13: slot that gets triggered from context menu -- define tag
void EditorTagManager::s_newTag(){
    DefineTag();
}

// ###################################################################################################

// 6/14/13: slot that gets triggered from context menu -- revert tags
void EditorTagManager::s_revertTag(){
    RevertTags();
}

// ###################################################################################################

// 6/13/13: Slot that controls the pop-up context menu in the tag list
void EditorTagManager::showPopup(){
    QPoint p=QCursor::pos();
    contextmenu->popup(p);
}

// ###################################################################################################

void EditorTagManager::on_NewTag_clicked()
{
    DefineTag();
}

// ###################################################################################################

void EditorTagManager::on_RemoveTag_clicked()
{
    RemoveTag();
}

// ###################################################################################################

void EditorTagManager::on_TagList_itemClicked(QListWidgetItem *item)
{
    if(item->isSelected()){
        ui->RemoveTag->setEnabled(true);
        remove->setEnabled(true);
    }
}

// ###################################################################################################

void EditorTagManager::on_RevertTags_clicked()
{
    RevertTags();
}
