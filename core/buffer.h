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


#ifndef BUFFER_H
#define BUFFER_H
#include <QtGui/QApplication>
#include <QList>
#include <QStringList>
#include <QDate>

class Buffer
{
public:
    Buffer();
    //void OutputBuffer();

    static QString host;
    static QString database_name;
    static QString username;
    static QString password;

    static bool login_succeeded; // did the login attempt work?


    static QString defaulthost; // default host
    static QString defaultuser; // default user
    static QString defaultdatabase; // default db
    static QString backend; // database backend var
    static int databaseport; // database port to use for connections
    static int entryrange; // number of years to show, only used if allentries==false
    static QString full_name; // ful lname of the user. (new for 0.4)
    static int toolbar_pos; // Toolbar position... 0=left; 1=top; 2=right

    static bool alwaysusedefaults; // always use default values if true
    static bool allowroot; // allow root logins if true
    static bool allentries; // show all entries if true
    static bool showwarnings; // show warning dialogs if true
    static bool sortbyday; // sort list entires by day if true, by month if false
    static bool SSL; // use SSL encryption
    static bool use_indicator; // show entry indicator
    static bool needs_upgrade;
    static bool is_male; // Gender flag
    static bool firstrun; // decide if the program is firstrun or not

    static bool alternate_rows; //whether to show alternate row colors
    static bool rich_text; // whether to support rich text
    static bool use_background; // whether to support a background image
    static bool datebox_override; // use system colors if custom theme is active.
    static bool keep_time; // keep  entry time
    static bool use_system_time; // use system time if true, use remote server time if false
    static bool use_24_hour; // use 24 hour clock
    static bool autoload; // autoload most recent journal entry
    static bool show_icon_labels; // should toolbar icons have text labels? checked == yes, unchecked == no


    // date format: 0=International, 1=USA, 2=Japan
    static int date_format;

    // theme options; if true, use custom colors, if false use system theme
    static bool use_custom_theme;
    static bool use_custom_theme_editor;

    static bool tile_bg; // tile background if true
    static bool set_tree_background; // use background_image on entry tree if true;

    static QString text_hexcolor; // font color in hex format (#FFFFFF)
    static QString background_image; // path to background image on disk
    static bool show_title; // whether to show title for entries
    static QString font_face; // name of font to use in entries
    static QString font_size; //point size of font to use


    static bool editmode; // Start Editor in Edit Mode if true
    static QString editentry; // store entry id to be edited


    static QString version; // keep track of RoboJournal Version;

    // new for 0.4
    static bool use_full_name;
    static bool use_highlight;
    static bool trim_whitespace;
    static bool use_smart_quotes;
    static bool use_html_hyphens;
    static bool bg_is_fixed;
    static bool use_rounded_corners;
    static bool use_system_colors;
    static bool include_tags_in_export;
    static QString header_font;
    static QString body_font;
    static bool header_use_em;
    static bool body_use_em;
    static QString header_font_size;
    static QString body_font_size;
    static int highlight_color;
    static bool show_spell_errors_by_default;
    static bool use_spellcheck;
    static QString current_dictionary;
    static QString current_dictionary_aff;
    static bool use_dow;
    static QSize editor_geometry;
    static QSize config_geometry;
    static QSize preview_geometry;
    static QByteArray mainwindow_geometry;
    static bool use_misc_processing;

    //Buffers for output pane
    static QString LastSearchTerm;
    static QString EntryText;
    static QString SearchText;
    static int entry_scroll_pos;
    static int search_scroll_pos;
    static bool On_Search; // returns true if the search tab is active

    /*

    Remember if toolbar widgets were enabled. This is required for switching between search mode and entry
    mode.

    */

    static bool write_enabled;
    static bool edit_enabled;
    static bool tag_enabled;
    static bool delete_enabled;
    static bool export_enabled;
    static bool disconnect_enabled;
    static bool latest_enabled;
    static bool previous_enabled;
    static bool next_enabled;

    static QSize editor_temporarysize;
    static QSize config_temporarysize;

    static bool editor_sizechanged;
    static bool config_sizechanged;

    static bool updating_dictionary; // not used!

    static int entry_node_icon;

    // new for 0.4.1

    static bool name_in_titlebar;
    static bool show_untagged_reminder;
    static QString reminder_choice; //id number used for tagger in reminder mode
    static QString reminder_title; // title used for current entry (shown in tagger titlebar)
    static QList<QStringList> records;

    static bool show_reminder_next_time; // save value of the checkbox on the Tag Reminder Window.

    // new for 0.5 (6/5/13)
    static bool use_my_journals;
    static QString sqlite_default;
    static QStringList sqlite_favorites;
    static QByteArray mw_splitter_size; // added 6/21/13

    static bool use_date_override;
    static QDate override_date;
    static QString mysqldump_path_win; // added 9/2/13.
    static bool no_safety;
    static bool system_dic; // 11/1/13.

};

#endif // BUFFER_H
