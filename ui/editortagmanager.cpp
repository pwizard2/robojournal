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
#include <QStyledItemDelegate>
#include <QPainter>
#include "sql/mysqlcore.h"

TagListDelegate::TagListDelegate(QObject *parent)
    : QStyledItemDelegate(parent){

}

QSize TagListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize ret = QStyledItemDelegate::sizeHint(option, index);
    ret=ret * 1.3; // add some more padding between items

#ifdef _WIN32
    ret=ret * 1.4; // slightly more padding is needed for Windows
#endif
    return ret;
}


void TagListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const{


    // figure out how wide the line should be (8/11/13).
    QPalette palette;
    QRect rect=option.rect;
    QPoint start(rect.left(),rect.bottom());
    QPoint end(rect.right(), start.y());

    // draw a 50% opaque line using the current color scheme's "Mid" color.
    painter->setPen(QPen(palette.color(QPalette::Disabled, QPalette::Mid),
                         0.50, Qt::SolidLine, Qt::RoundCap));

    painter->drawLine(start,end);

    // Paint each row item.
    QStyledItemDelegate::paint(painter, option, index);
}

TagListDelegate::~TagListDelegate(){

}

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
// Darken the input color and return it as a QColor value (8/13/13). This is used to help active (selected)
// tags stand out in the tag list because the regular selected background color doesn't provide enough contrast.
QColor EditorTagManager::adjustColor(QColor input){

    QColor output=input.dark(145);
    return output;
}

// ###################################################################################################
// Allow the user to declare the input tag if it does not already exist in the tag list. 7/31/13.
void EditorTagManager::EasyDeclareTag(QString input){

    QMessageBox m;

    // Bugfix for 0.4.1 (3/5/13): Replace simple operator check with a "smarter" regexp.
    // The user should NEVER be allowed to declare "null" (case insensitive) as a tag
    // because that is a reserved word in the tagging system; entries marked with Null have
    // "No tags for this post" as their tag data.
    QRegExp banned("null", Qt::CaseInsensitive);

    if(banned.exactMatch(input)){

        m.critical(this,"RoboJournal","You are not allowed to declare \"" + input +
                   "\" (or any other uppercase/lowercase variant of it) because it is a reserved keyword.");
        input.clear();
    }
    else{

        const QIcon newicon(":/icons/tag_red_add.png");

        int choice=m.question(this,"RoboJournal","\"" + input + "\" is not on the list. Do you wish to add it as a new tag?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if((choice==QMessageBox::Yes) && (!input.isEmpty())){

            QTreeWidgetItem *defined=new QTreeWidgetItem();
            defined->setText(0,input);
            defined->setIcon(0,newicon);
            defined->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            defined->setCheckState(0, Qt::Unchecked);

            ui->AvailableTags->insertTopLevelItem(0, defined);
        }
    }
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

    if(Buffer::showwarnings){
        QMessageBox m;
        int choice=m.question(this,"RoboJournal","Do you really want to discard the changes you made to this entry\'s tags?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(choice==QMessageBox::Yes){

            // purge all current checked items...
            QTreeWidgetItemIterator it(ui->AvailableTags);

            while(*it){
                QTreeWidgetItem *current=*it;

                current->setFont(0, nonselected);
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
            emit Sig_Revert_Off();

            // use TagListDelegate to draw lines between list items
            ui->AvailableTags->setItemDelegate(new TagListDelegate(this));
        }
    }

    else{

        // purge all current checked items...
        QTreeWidgetItemIterator it(ui->AvailableTags);

        while(*it){
            QTreeWidgetItem *current=*it;

            current->setFont(0, nonselected);
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
        emit Sig_Revert_Off();
    }
}

// ###################################################################################################

// 6/13/13: Add a new tag to the AvailableTags list. This uses the new TaggingShared class.
void EditorTagManager::DefineTag(){

    // get the latest batch of tags just in case new ones have been added since the form was loaded
    QStringList current_list;
    QTreeWidgetItemIterator it(ui->AvailableTags);
    const QIcon newicon(":/icons/tag_red_add.png");

    while(*it){
        QTreeWidgetItem *current=*it;
        current_list.append(current->text(0));
        ++it;
    }

    TaggingShared ts;
    QString tag=ts.DefineTag(current_list);

    if(!tag.isEmpty()){
        QTreeWidgetItem *defined=new QTreeWidgetItem();
        defined->setText(0,tag);
        defined->setIcon(0,newicon);
        defined->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        defined->setCheckState(0, Qt::Unchecked);

        ui->AvailableTags->insertTopLevelItem(0, defined);
    }
}

// ###################################################################################################

// 6/10/13: Create toolbar and layout for this class.
void EditorTagManager::PrimaryConfig(){

    // Establish font colors for [un]selected items based on current OS Palette.
    const QPalette pal;
    const QBrush bg=pal.midlight();
    const QBrush fg=pal.highlight();

    const QBrush p_bg=pal.base();
    const QBrush p_fg=pal.windowText();

    selected_bg=bg.color();
    selected_fg=adjustColor(fg.color());

    plain_bg=p_bg.color();
    plain_fg=p_fg.color();

    // set font global qualities for selected tags.
    selected.setWeight(QFont::Black);
    //selected.setUnderline(true);
    selected.setStyleStrategy(QFont::PreferAntialias);

    // Do the same for non-selected tag font.
    nonselected.setWeight(QFont::Normal);
    //nonselected.setUnderline(false);
    nonselected.setStyleStrategy(QFont::PreferAntialias);

    // Create toolbar.
    QToolBar *bar = new QToolBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    bar->setLayoutDirection(Qt::LeftToRight);
    bar->setContextMenuPolicy(Qt::DefaultContextMenu);

    const QFont toolbarFont("Sans",7);
    bar->setFont(toolbarFont);

    const QFont grepFont("Sans",9);
    ui->GrepBox->setFont(grepFont);

    if(Buffer::show_icon_labels){
        ui->NewTag->setText("Define Tag");
        ui->RevertTags->setText("Revert Tags");

        ui->NewTag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->StripTags->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->RevertTags->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->AutoTagButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }

    // Populate toolbar with loose UI elements.
    bar->addWidget(ui->NewTag);
    bar->addWidget(ui->StripTags);
    bar->addWidget(ui->AutoTagButton);
    bar->addSeparator();
    bar->addWidget(ui->GrepBox);
    bar->addWidget(ui->ClearButton);

    //tighten up toolbar spacing for 0.5 (7/15/13).
    const QSize barSize(16,16);
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

    // Bugfix (9/1/13): Disable Revert Tags button to start with. Signals and Slots should control when it should be enabled.
    ui->RevertTags->setDisabled(true);

    connect(ui->GrepBox, SIGNAL(returnPressed()), this, SLOT(query()));
    connect(this, SIGNAL(Sig_Revert_Off()), this,SLOT(Revert_Off()));
    connect(this, SIGNAL(Sig_Revert_On()), this,SLOT(Revert_On()));

    // use TagListDelegate to draw lines between list items.
    ui->AvailableTags->setItemDelegate(new TagListDelegate(this));

    if((Buffer::editmode) && (EditorTagManager::standalone_tagger))
        ui->AutoTagButton->setDisabled(true);
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

            // Set the correct marker on every top-level node. The delegate uses this data to determine which nodes
            // (i.e top-level nodes) should have separators between them. Do not put separators between child nodes
            // because the user needs to understand they belong to the parent (8/12/13).
            if(!next->parent()){
                // Store the top-level marker in the space for status-bar role text. We're not using that for anything anyway.
                next->setData(0,4,"ROOT-LEVEL");
            }
        }
    }
}

// ###################################################################################################

// (6/14/13) Fetch current tags from database. This is NOT to be confused with the
// public method that Exports the tags.
void EditorTagManager::LoadTags(QString id){

    TaggingShared ts;
    QStringList tags=ts.FetchTags(id);

    QTreeWidgetItemIterator it(ui->AvailableTags);

    while(*it){

        QTreeWidgetItem *current=*it;

        if(tags.contains(current->text(0))){
            current->setCheckState(0,Qt::Checked);

            current->setFont(0, selected);
            //current->setBackgroundColor(0, selected_bg);
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
    if(Qt::Checked == item->checkState(0)){
        item->setCheckState(0, Qt::Unchecked);
        item->setFont(0, nonselected);
        item->setForeground(0, plain_fg);
    }
    else{
        item->setCheckState(0, Qt::Checked);
        item->setFont(0, selected);
        item->setForeground(0,selected_fg);
    }

    emit Sig_UnlockTaggerApplyButton();

    if((Buffer::editmode) || (standalone_tagger))
        emit Sig_Revert_On();
}


// ###################################################################################################
void EditorTagManager::on_StripTags_clicked()
{
    if(Buffer::showwarnings){
        QMessageBox m;

        int choice=m.question(this,"RoboJournal","Do you really want to remove all tags from this entry?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(choice==QMessageBox::Yes){

            // purge all current checked items...
            QTreeWidgetItemIterator it(ui->AvailableTags);

            while(*it){
                QTreeWidgetItem *current=*it;

                current->setFont(0, nonselected);
                //current->setBackgroundColor(0, plain_bg);
                current->setForeground(0, plain_fg);
                current->setCheckState(0,Qt::Unchecked);

                it++;
            }
            ui->AvailableTags->repaint();

            // Emit signals that unlock the apply Button (if this is encased in a Tagger object) and unlock the Revert Tags button.
            // 9/1/2013
            emit Sig_UnlockTaggerApplyButton();

            if((Buffer::editmode) || (standalone_tagger))
                emit Sig_Revert_On();

        }
    }
    else{

        // purge all current checked items...
        QTreeWidgetItemIterator it(ui->AvailableTags);

        while(*it){
            QTreeWidgetItem *current=*it;

            current->setFont(0, nonselected);
            //current->setBackgroundColor(0, plain_bg);
            current->setForeground(0, plain_fg);
            current->setCheckState(0,Qt::Unchecked);

            it++;
        }

        // Emit signals that unlock the apply Button (if this is encased in a Tagger object) and unlock the Revert Tags button.
        // 9/1/2013
        emit Sig_UnlockTaggerApplyButton();
        emit Sig_Revert_On();
    }

}

// ###################################################################################################
void EditorTagManager::query(){

    QString input=ui->GrepBox->text();

    QStringList tags;

    QTreeWidgetItemIterator it(ui->AvailableTags);

    while(*it){
        QTreeWidgetItem *current=*it;
        tags.append(current->text(0));
        it++;
    }

    // give the user a chance to declare the tag if it is not in the list
    if(!tags.contains(input)){
        EasyDeclareTag(input);
    }

    // otherwise, search for it.
    else{
        // call to filter search function goes here eventually.
    }
}

// ###################################################################################################
void EditorTagManager::Revert_Off(){
    ui->RevertTags->setDisabled(true);
}

// ###################################################################################################
void EditorTagManager::Revert_On(){
    ui->RevertTags->setDisabled(false);
}

// ###################################################################################################
// This function uses regular expressions to check the body text for words that match existing tags
// and checks all matching tags it finds. This saves the user the trouble of having to go through the
// list by hand. New for 0.5, --Will Kraft (10/10/13).
void EditorTagManager::AutoTag(QString id){

    //Clear all current tags
    // purge all current checked items...
    QTreeWidgetItemIterator it(ui->AvailableTags);

    while(*it){
        QTreeWidgetItem *current=*it;

        current->setFont(0, nonselected);
        //current->setBackgroundColor(0, plain_bg);
        current->setForeground(0, plain_fg);
        current->setCheckState(0,Qt::Unchecked);

        it++;
    }


    // First, get the list of tags to check
    TaggingShared ts;
    QStringList available_tags=ts.TagAggregator();

    // Next, get the body text for the current entry. Editor::body should hold it if the entry is open
    // in the Editor, otherwise pull it directly from the database.
    QString body=Editor::body;

    if(Editor::body.isEmpty()){

        // Pull data from a MySQL/MariaDB database
        if(Buffer::backend=="MySQL"){

            MySQLCore m;
            QList<QString> list=m.RetrieveEntryFull(id);

            body=list.at(5);

            // Replace post-processing features with plain text to make the entry easy to edit. This will be a temporary
            // work-around until Rich Text is fully supported. It would not do to have people see &mdash; and wonder what it is.
            // The special characters will be restored anyway once the user saves the entry.
            body=body.replace("&ldquo;","\"");
            body=body.replace("&rdquo;","\"");
            body=body.replace("&mdash;","--");
            body=body.replace("&ndash;","-");
            body=body.replace("&hellip;","...");
            body=body.replace("&rsquo;","\'");
            body=body.replace(QRegExp("</?sup>"),"");

            //0.5 bugfix (9/8/13) Convert nbsp back to tab stops.
            body=body.replace(QRegExp("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"), "\t");
        }
    }

    // QStringList "matches" holds all matching tags in the body text.
    QStringList matches;

    // Loop through the list of available tags and look for all occurences of a word.
    for(int i=0; i<available_tags.size(); i++){

        QString next_item=available_tags.at(i);

        if(body.contains(next_item,Qt::CaseInsensitive))
            matches << next_item;
    }

    // Automatically check all matching items in the Available Tags list.
    for(int j=0; j < matches.size(); j++){

        QString next_match=matches.at(j);

        QTreeWidgetItemIterator it2(ui->AvailableTags);
        while(*it2){

            QTreeWidgetItem *current=*it2;

            if(current->text(0)==next_match){
                current->setCheckState(0,Qt::Checked);
                current->setFont(0, selected);
                current->setForeground(0,selected_fg);
            }

            it2++;
        }
    }

    QMessageBox m;
    if(matches.isEmpty()){
        m.critical(this,"RoboJournal","RoboJournal could not find any matching tags to apply.");
    }
    else{
        m.information(this,"RoboJournal","RoboJournal scanned the entry and automatically applied "
                      + QString::number(matches.size()) + " tag(s).");
    }
}

// ###################################################################################################
void EditorTagManager::on_AutoTagButton_clicked()
{
    if((!Buffer::editmode) && (!EditorTagManager::standalone_tagger)){
        // Eventually add code to process an unsaved entry but for now just return to prevent a segfault (10/10/13).
        std::cout << "OUTPUT: Unsaved entries don't support auto-tagging yet!" << std::endl;
        return;
    }
    else{

        if(EditorTagManager::standalone_tagger){
            emit Sig_UnlockTaggerApplyButton();

            AutoTag(Tagger::id_num);
        }
        else{
            AutoTag(Buffer::editentry);
        }

        // Sig_LockTaggerApplyButton();
        emit Sig_Revert_On();
    }
}
