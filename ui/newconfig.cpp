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
*/

#include "ui/newconfig.h"
#include "ui_newconfig.h"
#include <QIcon>
#include <QSize>
#include <iostream>
#include "ui/configurationgeneral.h"
#include "ui_configurationgeneral.h"
#include "ui/configurationjournal.h"
#include "ui_configurationjournal.h"
#include "ui/configurationmysql.h"
#include "ui_configurationmysql.h"
#include "ui/configurationappearance.h"
#include "ui_configurationappearance.h"
#include "ui/configurationeditor.h"
#include "ui_configurationeditor.h"
#include "ui/configurationexport.h"
#include "ui_configurationexport.h"
#include "ui/configurationsqlite.h"
#include "ui_configurationsqlite.h"
#include <QStackedWidget>
#include "core/buffer.h"
#include <QGraphicsView>
#include <QDebug>
#include <core/settingsmanager.h>

Newconfig::Newconfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Newconfig)
{
    ui->setupUi(this);
    startup=true;

    // setup form
    PrimaryConfig();

}


Newconfig::~Newconfig()
{

    delete ui;
}



// new default settings that can be called from any class through Config::$var.
// These should be null unless initiated elsewhere.
// even bool Newconfig::s and ints should be treated as QStrings b/c they are written to a config file for later use.

QString Newconfig::new_default_host;
QString Newconfig::new_default_user;
QString Newconfig::new_default_db;
QString Newconfig::new_default_port;
bool Newconfig::new_always_use_defaults;
bool Newconfig::new_show_confirm;
QString Newconfig::new_dbtype;
bool Newconfig::new_allow_root;
int Newconfig::new_entry_range;
bool Newconfig::new_show_all_entries;
int Newconfig::new_toolbar_pos;
bool Newconfig::new_sort_by_day;

// new for version 0.2
bool Newconfig::new_alternate_rows;
bool Newconfig::new_rich_text;
bool Newconfig::new_use_background;
bool Newconfig::new_use_custom_theme;
QString Newconfig::new_text_hexcolor;
QString Newconfig::new_background_image;
bool Newconfig::new_show_title;
QString Newconfig::new_font_face;
int Newconfig::new_font_size;
int Newconfig::new_date_mode;
bool Newconfig::new_tile_bg;
bool Newconfig::new_use_background_in_tree;
bool Newconfig::new_record_time;
bool Newconfig::new_use_local_time;
bool Newconfig::new_24_hr;
bool Newconfig::new_icon_labels;
bool Newconfig::new_autoload;
bool Newconfig::new_SSL;
bool Newconfig::new_indicator;
bool Newconfig::new_datebox_override;
bool Newconfig::new_use_full_name;
bool Newconfig::new_use_highlights;
bool Newconfig::new_trim_whitespace;
bool Newconfig::new_use_smart_quotes;
bool Newconfig::new_use_html_hyphens;
bool Newconfig::new_bg_is_fixed;
bool Newconfig::new_gender_is_male;
bool Newconfig::new_use_rounded_corners;
bool Newconfig::new_use_system_colors;
bool Newconfig::new_include_tags_in_export;
bool Newconfig::new_header_use_em;
bool Newconfig::new_body_use_em;

QString Newconfig::new_header_font;
QString Newconfig::new_body_font;
QString Newconfig::new_body_font_size;
QString Newconfig::new_header_font_size;
QString Newconfig::new_full_name;
int Newconfig::new_highlight_color;
bool Newconfig::new_use_custom_theme_editor;
bool Newconfig::new_show_spell_errors_by_default;
bool Newconfig::new_use_spellcheck;
QString Newconfig::new_current_dictionary;
QString Newconfig::new_current_dictionary_aff;
int Newconfig::new_entry_node_icon;
bool Newconfig::new_use_dow;
bool Newconfig::new_use_misc_processing;

// new for 0.4.1
bool Newconfig::new_name_in_titlebar;
bool Newconfig::new_show_untagged_reminder;

// new for 0.5 (6/5/13)
bool Newconfig::new_use_my_journals;
QStringList Newconfig::new_sqlite_favorites;
QString Newconfig::new_sqlite_default;

//##################################################################################
// This function sets up the form (creates categories for list, etc.)
void Newconfig::PrimaryConfig(){
    using namespace std;

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

//    int width=this->width();
//    int height=this->height();
//    this->setMaximumSize(width,height);
//    this->setMinimumSize(width,height);

    // set up category list (icons and entries)
    QIcon general(":/icons/application.png");
    QIcon behavior(":/icons/book.png");
    QIcon editor(":/icons/ui-text-area.png");
    QIcon appearance(":/icons/picture.png");
    QIcon disk(":/icons/external.png");
    QIcon mysql(":/icons/mysql_icon2.png");
    QIcon sqlite(":/icons/sqlite_icon.png");

    ui->Categories->clear();

    // Set vertical spacing for Category Items
    QSize space(0,25);


    QTreeWidgetItem *settings_general = new QTreeWidgetItem(ui->Categories);
    settings_general->setIcon(0, general);
    settings_general->setText(0,"General");
    settings_general->setSizeHint(0,space);

    QTreeWidgetItem *settings_behavior = new QTreeWidgetItem(ui->Categories);
    settings_behavior->setIcon(0, behavior);
    settings_behavior->setText(0,"Journal");
    settings_behavior->setSizeHint(0,space);


    QTreeWidgetItem *settings_editor = new QTreeWidgetItem(ui->Categories);
    settings_editor->setIcon(0, editor);
    settings_editor->setText(0,"Editor");
    settings_editor->setSizeHint(0,space);

    QTreeWidgetItem *settings_appearance = new QTreeWidgetItem(ui->Categories);
    settings_appearance->setIcon(0, appearance);
    settings_appearance->setText(0,"Appearance");
    settings_appearance->setSizeHint(0,space);

    QTreeWidgetItem *settings_export = new QTreeWidgetItem(ui->Categories);
    settings_export->setIcon(0, disk);
    settings_export->setText(0,"Export Settings");
    settings_export->setSizeHint(0,space);

    QTreeWidgetItem *settings_mysql = new QTreeWidgetItem(ui->Categories);
    settings_mysql->setIcon(0, mysql);
    settings_mysql->setText(0,"MySQL Settings");
    settings_mysql->setSizeHint(0,space);

    // New SQLite page for version 0.5
    QTreeWidgetItem *settings_sqlite = new QTreeWidgetItem(ui->Categories);
    settings_sqlite->setIcon(0, sqlite);
    settings_sqlite->setText(0,"SQLite Settings");
    settings_sqlite->setSizeHint(0,space);

    // Create a StackedWidget. The stack holds all the "pages" and (most importantly) remembers the current state of each.
    // The stack shows only one page at a time. When an item in the category list (General, Journal, etc.) is clicked
    // on, the stack loads the page associated with that category.
    // Special thanks to the Clementine source code for helping me figure out how to do this.
    stack=new QStackedWidget(this);

    // Instantiate the pages when they get added to the stack. This allows each one to be independently sized.
    // Otherwise,  all the pages inherit the same size as the longest one. (producing uneeded scrollbars)
    stack->addWidget(g=new ConfigurationGeneral(this));
    stack->addWidget(j=new ConfigurationJournal(this));
    stack->addWidget(e=new ConfigurationEditor(this));
    stack->addWidget(a=new ConfigurationAppearance(this));
    stack->addWidget(x=new ConfigurationExport(this));
    stack->addWidget(m=new ConfigurationMySQL(this));
    stack->addWidget(s=new ConfigurationSQLite(this));

    // Limit how much the width of the window can be compressed. New for 0.5 (7/2/13).
    ui->PageArea->setMinimumWidth(g->width());

    // put the stack in the ScrollArea.
    ui->PageArea->setWidget(stack);
    ui->PageArea->setWidgetResizable(true);
    ui->PageArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PageArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->Categories->setCurrentItem(settings_general);

    // Fix bug where a scrollbar appears on pages that don't need it. This code tells the stack that it's ok
    // to have different-sized widgets. The loop should start on Widget 1 because Widget 0 can have a squished
    // appearance if we set its size policy to ignore.
    for(int i=1; i<stack->count(); i++){
        stack->setCurrentIndex(i);
        stack->currentWidget()->setSizePolicy(QSizePolicy::Ignored,
                                              QSizePolicy::Ignored);
        adjustSize();
    }

    // Ensure the General Widget is visible when window opens.
    stack->setCurrentIndex(0);

    // restore window size from previous session
    bool usetemp=!Buffer::config_temporarysize.isEmpty();
    //cout << " Temporary size is valid : " << usetemp << endl;

    if(usetemp && startup){

        resize(Buffer::config_temporarysize);
    }
    else{
        resize(Buffer::config_geometry);
    }

    startup=false;

    // Add necessary padding to ui elements. New for 0.5.
    ui->buttonBox->setContentsMargins(0,0,9,9);

}

void Newconfig::resizeEvent(QResizeEvent *){

    if(!startup){
        Buffer::config_temporarysize=this->size();
        Buffer::config_sizechanged=true;
    }
}


void Newconfig::on_Categories_currentItemChanged(QTreeWidgetItem *current)
{
    int idx=ui->Categories->currentIndex().row();

    QString category=current->text(0);

    this->setWindowTitle("Preferences - " + category);

    if (stack->currentWidget() != 0) {
        stack->currentWidget()->setSizePolicy(QSizePolicy::Ignored,
                                              QSizePolicy::Ignored);
    }
    stack->setCurrentIndex(idx);
    stack->currentWidget()->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Expanding);

    // Have the window keep its current size when we change  pages
    resize(this->size());

    // Although the window gets resized, a page change should NOT count as a resize event.
    // This fixes a bug which causes the Editor to revert to its default 640x480 size if the
    // user changes too many pages. For some reason, the size is not being saved here
    // so it's better to ignore these resize events.
    //Buffer::config_sizechanged=false;
}

void Newconfig::UpdateData(){

    // fetch new form data from each page
    g->GetChanges();
    j->GetChanges();
    e->GetChanges();
    a->GetChanges();
    x->GetChanges();
    m->GetChanges();

    // new for 0.5:
    s->GetChanges();

    MadeChanges=true;

    // update the settings
    SettingsManager sm;
    sm.UpdateConfig();
}


void Newconfig::on_buttonBox_accepted()
{
    UpdateData();
}



