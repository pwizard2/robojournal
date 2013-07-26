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
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QColor>

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

    QTreeWidgetItemIterator it(ui->AvailableTags);

    while(*it){

        QTreeWidgetItem *current=*it;

        if(Qt::Checked == current->checkState(0)){
            taglist.append(current->text(0));
        }

        it++;
    }

    QString tags=taglist.join(";");
    return tags;
}

// ###################################################################################################
// 6/14/13: Revert tags to their default values. This method is only accessible if the Editor
// is in Edit Mode (since new entries have no pre-existing data to revert to).
void EditorTagManager::RevertTags(){

    QFont normal_font;
    normal_font.setBold(false);

    if(Buffer::showwarnings){
        QMessageBox m;
        int choice=m.question(this,"RoboJournal","Do you really want to discard the changes you made to this entry\'s tags?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(choice==QMessageBox::Yes){

            // purge all current checked items...
            QTreeWidgetItemIterator it(ui->AvailableTags);

            while(*it){
                QTreeWidgetItem *current=*it;

                current->setFont(0, normal_font);
                current->setBackgroundColor(0, plain_bg);
                current->setForeground(0, plain_fg);
                current->setCheckState(0,Qt::Unchecked);

                it++;
            }

            // ... And regenerate the saved list.
            if(EditorTagManager::standalone_tagger){
                emit Sig_UnlockTaggerApplyButton();

                LoadTags(Tagger::id_num);
            }
            else{
                LoadTags(Buffer::editentry);
            }

            emit Sig_LockTaggerApplyButton();
        }
    }

    else{

        // purge all current checked items...
        QTreeWidgetItemIterator it(ui->AvailableTags);

        while(*it){
            QTreeWidgetItem *current=*it;

            current->setFont(0, normal_font);
            current->setBackgroundColor(0, plain_bg);
            current->setForeground(0, plain_fg);
            current->setCheckState(0,Qt::Unchecked);
            it++;
        }

        // ... And regenerate the saved list.
        if(EditorTagManager::standalone_tagger){
            emit Sig_UnlockTaggerApplyButton();
            LoadTags(Tagger::id_num);
        }
        else{
            LoadTags(Buffer::editentry);
        }

        emit Sig_LockTaggerApplyButton();
    }
}

// ###################################################################################################

// 6/13/13: Add a new tag to the AvailableTags list. This uses the new TaggingShared class.
void EditorTagManager::DefineTag(){

    // get the latest batch of tags just in case new ones have been added since the form was loaded
    QStringList current_list;
    QTreeWidgetItemIterator it(ui->AvailableTags);
    QIcon newicon(":/icons/tag_red_add.png");

    while(*it){
        QTreeWidgetItem *current=*it;
        current_list.append(current->text(0));
        ++it;
    }

    TaggingShared ts;
    QString tag=ts.DefineTag(current_list);

    if(!tag.isEmpty()){
        QTreeWidgetItem *defined=new QTreeWidgetItem(ui->AvailableTags);
        defined->setText(0,tag);
        defined->setIcon(0,newicon);
        defined->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        defined->setCheckState(0, Qt::Unchecked);
    }
}

// ###################################################################################################

// 6/10/13: Create toolbar and layout for this class.
void EditorTagManager::PrimaryConfig(){

    // find and use system colors for selected/clear tags.
    QPalette pal;

//#ifdef _WIN32
//    QBrush bg=pal.midlight();
//#endif;

//#ifdef unix
//    QBrush bg=pal.alternateBase();
//#endif

    QBrush bg=pal.midlight();
    QBrush fg=pal.buttonText();

    QBrush p_bg=pal.base();
    QBrush p_fg=pal.windowText();

    selected_bg=bg.color();
    selected_fg=fg.color();

    plain_bg=p_bg.color();
    plain_fg=p_fg.color();

    // Create toolbar.
    QToolBar *bar = new QToolBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    bar->setLayoutDirection(Qt::LeftToRight);
    bar->setContextMenuPolicy(Qt::DefaultContextMenu);

    QFont toolbarFont("Sans",7);
    bar->setFont(toolbarFont);

    QFont grepFont("Sans",9);
    ui->GrepBox->setFont(grepFont);

    if(Buffer::show_icon_labels){
        ui->NewTag->setText("Define Tag");
        ui->RevertTags->setText("Revert Tags");

        ui->NewTag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->StripTags->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->RevertTags->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }

    // Populate toolbar with loose UI elements.
    bar->addWidget(ui->NewTag);
    bar->addWidget(ui->StripTags);
    bar->addSeparator();
    bar->addWidget(ui->GrepBox);
    bar->addWidget(ui->ClearButton);

    //tighten up toolbar spacing for 0.5 (7/15/13).
    QSize barSize(16,16);
    bar->setIconSize(barSize);
    bar->setContentsMargins(0,2,0,0);

    // Force everything into a vboxlayout so the tag interface stretches to fill all available space.
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(bar,0);
    layout->addWidget(ui->AvailableTags);

    this->setLayout(layout);

    // Get list of tags
    CreateTagList();

    // Add the fat spacer and the RevertTags button now because the layout has been applied
    // by this point. This means the spacer should stretch to fit the layout (docking the button on the right).
    //    QWidget* fat_spacer = new QWidget();
    //    fat_spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    //    bar->addWidget(fat_spacer);
    bar->addSeparator();
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
    ui->AvailableTags->setRootIsDecorated(0);

    // Bugfix for 0.5: fix bug that causes the first tag in the list to be omitted.
    // int z used to start at 1 for some reason but it's fixed now. --Will Kraft (7/23/13).
    for(int i=0; i < tags.size(); i++){

        if(!tags.at(i).isEmpty()){
            QTreeWidgetItem *next=new QTreeWidgetItem(ui->AvailableTags);
            next->setText(0,tags.at(i));
            next->setIcon(0,tagicon);
            next->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            next->setCheckState(0, Qt::Unchecked);
        }
    }
}

// ###################################################################################################

// (6/14/13) Fetch current tags from database. This is NOT to be confused with the
// public method that Exports the tags.
void EditorTagManager::LoadTags(QString id){

    TaggingShared ts;
    QStringList tags=ts.FetchTags(id);

    QFont bold_font;
    bold_font.setBold(true);

    QTreeWidgetItemIterator it(ui->AvailableTags);

    while(*it){

        QTreeWidgetItem *current=*it;

        if(tags.contains(current->text(0))){
            current->setCheckState(0,Qt::Checked);

            current->setFont(0, bold_font);
            current->setBackgroundColor(0, selected_bg);
            current->setForeground(0,selected_fg);
        }

        it++;
    }
}

// ###################################################################################################

void EditorTagManager::on_NewTag_clicked()
{
    DefineTag();
}

// ###################################################################################################

void EditorTagManager::on_RevertTags_clicked()
{
    RevertTags();
}

// ###################################################################################################
void EditorTagManager::on_AvailableTags_itemClicked(QTreeWidgetItem *item)
{
    QFont bold_font;
    bold_font.setBold(true);

    QFont normal_font;
    normal_font.setBold(false);

    if(Qt::Checked == item->checkState(0)){
        item->setCheckState(0, Qt::Unchecked);
        item->setFont(0, normal_font);
        item->setBackgroundColor(0, plain_bg);
        item->setForeground(0, plain_fg);
    }
    else{
        item->setCheckState(0, Qt::Checked);
        item->setFont(0, bold_font);
        item->setBackgroundColor(0, selected_bg);
        item->setForeground(0,selected_fg);
    }

    emit Sig_UnlockTaggerApplyButton();
}


// ###################################################################################################
void EditorTagManager::on_StripTags_clicked()
{

    QFont normal_font;
    normal_font.setBold(false);

    if(Buffer::showwarnings){
        QMessageBox m;

        int choice=m.question(this,"RoboJournal","Do you really want to remove all tags from this entry?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(choice==QMessageBox::Yes){

            // purge all current checked items...
            QTreeWidgetItemIterator it(ui->AvailableTags);

            while(*it){
                QTreeWidgetItem *current=*it;

                current->setFont(0, normal_font);
                current->setBackgroundColor(0, plain_bg);
                current->setForeground(0, plain_fg);
                current->setCheckState(0,Qt::Unchecked);

                it++;
            }

            emit Sig_UnlockTaggerApplyButton();
        }
    }
    else{

        // purge all current checked items...
        QTreeWidgetItemIterator it(ui->AvailableTags);

        while(*it){
            QTreeWidgetItem *current=*it;

            current->setFont(0, normal_font);
            current->setBackgroundColor(0, plain_bg);
            current->setForeground(0, plain_fg);
            current->setCheckState(0,Qt::Unchecked);

            it++;
        }

        emit Sig_UnlockTaggerApplyButton();
    }
}
