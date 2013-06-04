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

#ifndef NEWCONFIG_H
#define NEWCONFIG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "core/settingsmanager.h"
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
#include <QSize>


namespace Ui {
class Newconfig;
}

class Newconfig : public QDialog
{
    Q_OBJECT
    
public:
    explicit Newconfig(QWidget *parent = 0);
    ~Newconfig();
    bool MadeChanges;

    // new default settings that can be called from any class through Config::$var.
    // These should be null unless initiated elsewhere.
    // even bools and ints should be treated as QStrings b/c they are written to a config file for later use.

    static QString new_default_host;
    static QString new_default_user;
    static QString new_default_db;
    static QString new_default_port;
    static bool new_always_use_defaults;
    static bool new_show_confirm;
    static QString new_dbtype;
    static bool new_allow_root;
    static int new_entry_range;
    static bool new_show_all_entries;
    static int new_toolbar_pos;
    static bool new_sort_by_day;

    static bool new_use_spellcheck;
    static QString new_current_dictionary;
    static QString new_current_dictionary_aff;

    // new for version 0.2
    static bool new_alternate_rows;
    static bool new_rich_text;
    static bool new_use_background;
    static bool new_use_custom_theme;
    static QString new_text_hexcolor;
    static QString new_background_image;
    static bool new_show_title;
    static QString new_font_face;
    static int new_font_size;
    static int new_date_mode;
    static bool new_tile_bg;
    static bool new_use_background_in_tree;
    static bool new_record_time;
    static bool new_use_local_time;
    static bool new_24_hr;
    static bool new_icon_labels;
    static bool new_autoload;
    static bool new_SSL;
    static bool new_indicator;
    static bool new_datebox_override;
    static bool new_use_full_name;
    static bool new_use_highlights;
    static bool new_trim_whitespace;
    static bool new_use_smart_quotes;
    static bool new_use_html_hyphens;
    static bool new_bg_is_fixed;
    static bool new_gender_is_male;
    static bool new_use_rounded_corners;
    static bool new_use_system_colors;
    static bool new_include_tags_in_export;
    static bool new_header_use_em;
    static bool new_body_use_em;
    static bool new_show_spell_errors_by_default;
    static QString new_header_font;
    static QString new_body_font;
    static QString new_body_font_size;
    static QString new_header_font_size;
    static QString new_full_name;
    static int new_highlight_color;
    static bool new_use_custom_theme_editor;
    static int new_entry_node_icon;
    static bool new_use_dow;
    static bool new_use_misc_processing;

    //new for 0.4.1
    static bool new_name_in_titlebar;
    static bool new_show_untagged_reminder;

private slots:


    void on_Categories_currentItemChanged(QTreeWidgetItem *current);
    void on_buttonBox_accepted();
    void resizeEvent(QResizeEvent *);





private:
    Ui::Newconfig *ui;

    enum Role {
        Role_IsSeparator = Qt::UserRole
      };

    void PrimaryConfig();
    void LoadData();
    void UpdateData();

    ConfigurationGeneral *g;
    ConfigurationJournal *j;
    ConfigurationMySQL *m;
    ConfigurationAppearance *a;
    ConfigurationEditor *e;
    ConfigurationExport *x;
    ConfigurationSQLite *s; // New SQLite page for 0.5

    QStackedWidget *stack;

    QSize newsize;

    bool startup;
};

#endif // NEWCONFIG_H
