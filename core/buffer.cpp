/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
    

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


#include "core/buffer.h"
#include <QtGui/QApplication>

// Static variable declaration
QString Buffer::defaulthost;
QString Buffer::defaultuser;
QString Buffer::defaultdatabase;
int Buffer::databaseport;
int Buffer::entryrange;
QString Buffer::backend;

int Buffer::toolbar_pos;
bool Buffer::alwaysusedefaults;
bool Buffer::allowroot;
bool Buffer::allentries;
bool Buffer::showwarnings;
bool Buffer::sortbyday;
bool Buffer::firstrun;
bool Buffer::alternate_rows;
bool Buffer::rich_text;
bool Buffer::use_custom_theme;
bool Buffer::use_24_hour;
bool Buffer::use_background;
bool Buffer::use_system_time;
bool Buffer::tile_bg;
bool Buffer::set_tree_background;
bool Buffer::show_icon_labels;
bool Buffer::autoload;
bool Buffer::SSL;
bool Buffer::use_indicator;
bool Buffer::login_succeeded;
bool Buffer::is_male;
bool Buffer::datebox_override;

QString Buffer::username;
QString Buffer::password;
QString Buffer::host;
QString Buffer::database_name;
QString Buffer::background_image;
QString Buffer::font_size;
QString Buffer::text_hexcolor;
QString Buffer::font_face;
bool Buffer::keep_time;
bool Buffer::show_title;
int Buffer::date_format;
QString Buffer::full_name;
bool Buffer::needs_upgrade; // if true, journal needs to be upgraded from 0.1

bool Buffer::editmode; //if true, start the Editor in revision mode. If False, create a blank entry
QString Buffer::editentry; // the entry id that needs to be edited

// Current RoboJournal Version used globally in Program.
QString Buffer::version="0.5";

// new for 0.4
bool Buffer::use_full_name;
bool Buffer::use_highlight;
bool Buffer::trim_whitespace;
bool Buffer::use_smart_quotes;
bool Buffer::use_html_hyphens;
bool Buffer::bg_is_fixed;
bool Buffer::use_rounded_corners;
bool Buffer::use_system_colors;
bool Buffer::include_tags_in_export;
QString Buffer::header_font;
QString Buffer::body_font;
bool Buffer::header_use_em;
bool Buffer::body_use_em;
QString Buffer::header_font_size;
QString Buffer::body_font_size;
int Buffer::highlight_color;
bool Buffer::use_dow; // display date of week
QString Buffer::EntryText;
QString Buffer::SearchText;
int Buffer::entry_scroll_pos;
int Buffer::search_scroll_pos;
bool Buffer::On_Search;
QString Buffer::LastSearchTerm;
bool Buffer::use_custom_theme_editor;
bool Buffer::show_spell_errors_by_default;
bool Buffer::latest_enabled;
bool Buffer::previous_enabled;
bool Buffer::next_enabled;
bool Buffer::write_enabled;
bool Buffer::edit_enabled;
bool Buffer::export_enabled;
bool Buffer::tag_enabled;
bool Buffer::delete_enabled;
bool Buffer::disconnect_enabled;
bool Buffer::use_misc_processing;
bool Buffer::use_spellcheck;
QString Buffer::current_dictionary;
QString Buffer::current_dictionary_aff;

// These settings control the STORED STATIC sizes of resizable classes from last session.
// These are meant to be read and never changed.
QSize Buffer::editor_geometry;
QSize Buffer::config_geometry;
QSize Buffer::preview_geometry;
QByteArray Buffer::mainwindow_geometry;

// These settings control the DYNAMIC sizes of resizable classes SET DURING CURRENT RUNTIME.
// These values are updated whenever the size of the corresponding window gets changed. These size values (if !null)
// become the static sizes the next time the program runs. Yes, it's complicated.
QSize Buffer::editor_temporarysize;
QSize Buffer::config_temporarysize;
bool Buffer::config_sizechanged=false;
bool Buffer::editor_sizechanged=false;
bool Buffer::updating_dictionary;
int Buffer::entry_node_icon;

// new for 0.4.1
bool Buffer::name_in_titlebar;
bool Buffer::show_untagged_reminder;

// settings for tagger and tagger reminder window
QList<QStringList> Buffer::records;
QString Buffer::reminder_choice;
QString Buffer::reminder_title;
bool Buffer::show_reminder_next_time;

// new for 0.4.2
QByteArray Buffer::mw_splitter_size; // former 0.5 code but backported on 9/13/13.

// new for 0.5 (6/5/13)
QStringList Buffer::sqlite_favorites;
QString Buffer::sqlite_default;
bool Buffer::use_my_journals;
bool Buffer::open_editor;

// date override flag (8/23/13).
bool Buffer::use_date_override;
QDate Buffer::override_date;

//safety protocol override (9/13/13).
bool Buffer::no_safety;

// keep track of whether we're using system-level dictionaries (11/1/13).
bool Buffer::system_dic;

// Hold stored mysqldump location for Windows builds only. Linux/Unix always uses /usr/bin/mysqldump so this is not necessary (9/2/13).
QString Buffer::mysqldump_path_win;

// Remember if there was a failed login attempt this session and what the most recent database index was (12/28/13).
bool Buffer::remember_last;
int Buffer::last_host;
int Buffer::last_db;

// another last-minute addition, remembers the last page viewed in the help (7/20/14).
QString Buffer::helpdoc;

//

Buffer::Buffer()
{


}

/* deprecated
void Buffer::OutputBuffer(){

}
*/
