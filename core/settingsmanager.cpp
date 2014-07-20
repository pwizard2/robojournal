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

#include "core/settingsmanager.h"
#include "core/buffer.h"
#include <QSettings>
#include <QCoreApplication>
#include <iostream>
#include <QLineEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QAbstractButton>
#include "ui/firstrun.h"
#include "ui_firstrun.h"
#include <QPushButton>
#include <QDir>
#include <QFile>
#include "ui/newconfig.h"
#include "core/favoritecore.h"



/* This class is meant to be a full *replacement* of the ConfigManager class from RoboJournal 0.1-0.3.
 * Once SettingManager has been fully implemented, ConfigManager is to be deprecated and removed
 * from the source package. (deprecation completed by 2/26/13)
 */

SettingsManager::SettingsManager(){
    ok_param="[OK]";
    fail_param="[FAILED]";
}

//###################################################################################################
// Remember the page most recently viewed in the documentation window so the user can pick up where
// he/she left off on the previous session.  New for 0.5.  --Will Kraft (7/20/14).
void SettingsManager::Save_HelpDoc(QString path){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Behavior");
    settings.setValue("help_page", path);
    settings.endGroup();
}


//###################################################################################################
// Save the current path ($path) to MySQLDump. This is used exclusively on Windows so RoboJournal can
// remember where MySQLdump is. Unnecessary on linux b/c mysqldump is always stored in /usr/bin. --Will Kraft, (9/2/13).
void SettingsManager::Save_Mysqldump_Path(QString path){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Behavior");
    settings.setValue("mysqldump_path_win",path);
    settings.endGroup();

    // Save and reload config after making changes because this function is called during app runtime.
    settings.sync();
    LoadConfig(false);
}


//###################################################################################################
// Saves the current splitter position from the MainWindow. This allows someone to customize it once
// and have it stay that way. This should only be called when the mainwindow closes.
// New feature for 0.5. -- Will Kraft,  6/21/13

void SettingsManager::SaveSplitterPos(QByteArray value){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Behavior");
    settings.setValue("mw_splitter_position", value);
    settings.endGroup();
}

//###################################################################################################


// Save the behavior if the user opts to disable the tagging nag screen. This function should NOT
// be called from the preferences window!  New for 0.4.1; 2/26/13
void SettingsManager::SaveNagPreferences(){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Behavior");
    settings.setValue("show_untagged_reminder", Buffer::show_reminder_next_time);
    settings.endGroup();

    // Save and reload config after making changes because this function is called during app runtime.
    settings.sync();
    LoadConfig(false);

}

//###################################################################################################
// Why not remember the size of the Preferences window too? I get tired of scrolling.  New for 0.4.
void SettingsManager::SavePreviewSize(QSize geo){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Appearance");
    settings.setValue("preview_size", geo);
    settings.endGroup();
}



//###################################################################################################
// Why not remember the size of the Preferences window too? I get tired of scrolling.  New for 0.4.
void SettingsManager::SaveConfigSize(QSize geo){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Appearance");
    settings.setValue("config_size", geo);
    settings.endGroup();
}


//###################################################################################################
// Now that the Editor is resizable, remember editor's size for the future in case the user changed it.
// This code gets called right before the current Editor object gets destroyed. New for 0.4.
void SettingsManager::SaveEditorSize(QSize geo){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Appearance");
    settings.setValue("editor_size", geo);
    settings.endGroup();

    //settings.sync();

    // LoadConfig();
}

//###################################################################################################
// Just like SaveEditorsize, this remembers the current size of MainWindow.
// This code gets called right before the current MainWindow object gets destroyed. New for 0.4.
void SettingsManager::SaveMainWindowSize(QByteArray geo){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.beginGroup("Appearance");
    settings.setValue("mainwindow_size", geo);
    settings.endGroup();

    settings.sync();
}


//###################################################################################################
// install dictionaries to ~/.robojournal folder if necessary. This function makes sure the dictionaries
// are available and reinstalls them if necessary. New for 0.4.
void SettingsManager::InstallDictionaries(){

    using namespace std;

    QString path= QDir::homePath() + QDir::separator() + ".robojournal" + QDir::separator();
    path=QDir::toNativeSeparators(path);

    QFile EN_dict(path + "en_US.dic");
    QFile EN_dict_aff(path +  "en_US.aff");

    cout << "OUTPUT: Searching for default (US English) dictionary.........";

    if((!EN_dict.exists()) || (!EN_dict_aff.exists())){

        cout << fail_param << endl;

        QFile d1(":/en_US.dic");
        d1.copy(":/en_US.dic", path + "en_US.dic");

        QFile d2(":/en_US.aff");
        d2.copy(":/en_US.aff", path + "en_US.aff");

        cout << "OUTPUT: Reinstalled (English US) dictionaries to " << path.toStdString() << endl;
    }

    else{
        cout << ok_param << endl;

        if((!EN_dict.isWritable()) && (!EN_dict_aff.isWritable())){
            cout << "OUTPUT: Making sure the dictionary is writable................";
            EN_dict.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
            EN_dict_aff.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
            cout << ok_param << endl;
        }
        else{
            cout << "OUTPUT: Dictionary is properly writable. Proceeding with startup." << endl;
        }
    }
}

//###################################################################################################
// Get the user's full name
QString SettingsManager::FullName(){
    using namespace std;

    QString name = QInputDialog::getText(NULL, "RoboJournal",
                                         "Please enter your full name (as you would like it to be shown):", QLineEdit::Normal);

    //cout << "Name: " << name.toStdString() << endl;
    return name;
}

//###################################################################################################
// new gender check for 0.4
bool SettingsManager::GenderCheck(){

    using namespace std;

    QMessageBox m;
    m.setText("Please select your gender:");
    m.setWindowTitle("RoboJournal");
    m.setIcon(QMessageBox::Question);

    QPushButton *male=m.addButton("Male",QMessageBox::AcceptRole);
    QPushButton *female=m.addButton("Female",QMessageBox::AcceptRole);

    m.setStandardButtons(NULL);

    m.exec();

    bool isMale;
    if(m.clickedButton() == male){
        //cout << "User chose male" << endl;
        isMale=true;
    }

    if(m.clickedButton() == female){
        //cout << "User chose female" << endl;
        isMale=false;
    }

    return isMale;
}

//##################################################################################
// Create a new config with some default values plus others from the JournalCreator
// or JournalSelector classes.

void SettingsManager::NewConfig(QString host, QString db_name, QString port, QString newuser){
    using namespace std;

    QString name;
    bool isMale;

    // only show name/gender check if the app is in firstrun mode. Otherwise, use existing values in buffer.
    if(Buffer::firstrun){
        name=FullName();
        isMale=GenderCheck();
    }
    else{
        name=Buffer::full_name;
        isMale=Buffer::is_male;
    }

    QCoreApplication::setOrganizationName("Will Kraft");
    QCoreApplication::setApplicationName("robojournal");

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    QString folderpath=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator();

    settings.beginGroup("Backend");
    settings.setValue("default_host", host.trimmed());
    settings.setValue("default_db", db_name.trimmed());
    settings.setValue("default_user", newuser.trimmed());
    settings.setValue("db_type", "MySQL");
    settings.setValue("port", port);
    settings.setValue("entry_range", 4);
    settings.endGroup();

    // New SQLite options for RoboJournal 0.5
    settings.beginGroup("SQLite");
    settings.setValue("use_my_journals", true);
    settings.setValue("sqlite_favorites","");
    settings.setValue("sqlite_default","");
    settings.endGroup();

    settings.beginGroup("Behavior");
    settings.setValue("toolbar_location", 1);
    settings.setValue("allow_root_login", false);
    settings.setValue("show_all_entries", true);
    settings.setValue("always_use_defaults", true);
    settings.setValue("show_warnings", true);
    settings.setValue("sort_by_day", false);
    settings.setValue("alternate_colors", true);
    settings.setValue("use_time", true);
    settings.setValue("show_title", true);
    settings.setValue("use_24_hour_clock", false);
    settings.setValue("set_date_format", 1);
    settings.setValue("enable_rich_text", false);
    settings.setValue("use_dow", true);
    settings.setValue("open_editor", true);

    // Update for 0.5: never enable tooblar button text by default. This feature is actually
    // deprecated as of 6/21/13 because it wastes too much space. Undeprecated b/c the font issue has been solved now.
    if((!Buffer::show_icon_labels) && (!Buffer::firstrun)){
        settings.setValue("enable_toolbar_button_text", false);
    }
    else{
        settings.setValue("enable_toolbar_button_text", false);
    }

    settings.setValue("autoload_recent_entry", true);
    settings.setValue("ssl_support", false);
    settings.setValue("display_year_indicator", false);
    settings.setValue("use_highlights", true);
    settings.setValue("trim_whitespace", true);
    settings.setValue("smart_quotes", true);
    settings.setValue("html_hyphens", true);
    settings.setValue("use_spellcheck", false);
    settings.setValue("default_show_errors", false);

    // Bugfix: Keep dictionary and AFF blank in new configurations.
    settings.setValue("spellcheck_dictionary", "");
    settings.setValue("spellcheck_dictionary_aff", "");
    settings.setValue("system_dictionaries",false);
    settings.setValue("misc_processing",true);
    settings.setValue("name_in_titlebar", true);
    settings.setValue("show_untagged_reminder", true);
    settings.endGroup();

    settings.beginGroup("Appearance");
    settings.setValue("background_image", "");
    settings.setValue("use_background", false);
    settings.setValue("use_theme", false);
    settings.setValue("use_theme_editor",false);
    settings.setValue("tile_background", true);
    settings.setValue("background_tree", false);
    settings.setValue("background_fixed", true);
    settings.setValue("font_color", "#000000");
    settings.setValue("font_face", "");
    settings.setValue("font_size", 16);
    settings.setValue("datebox_override", false);
    settings.setValue("highlight_color", 0);
    settings.setValue("entry_node_icon", 0);
    settings.endGroup();

    settings.beginGroup("Export");
    settings.setValue("rounded_corners", true);
    settings.setValue("system_colors", false);
    settings.setValue("include_tags", true);
    settings.setValue("header_font", "Verdana");
    settings.setValue("body_font", "Times New Roman");
    settings.setValue("header_use_em", true);
    settings.setValue("body_use_em", true);
    settings.setValue("header_font_size", 3);
    settings.setValue("body_font_size", 1.2);
    settings.endGroup();

    settings.beginGroup("User");
    settings.setValue("user_gender_male", isMale);
    settings.setValue("user_full_name", name.trimmed());

    // bugfix 12/8/12: Do not set full name to true if no name was given.
    if(name.isEmpty()){
        settings.setValue("use_full_name", false);
    }
    else{
        settings.setValue("use_full_name", true);
    }

    settings.endGroup();


    // Save the settings
    settings.sync();

    // Clear Firstrun flag
    Buffer::firstrun=false;

    // Bugfix 12/8/12:
    // Install dictionaries now so they are available immediately after firstrun is finished.
    InstallDictionaries();

    //Create new favorites database. Meant to be a 5.0 bugfix to fix a problem that kept this
    // from happening during firstrun --Will Kraft (5/31/14).
    FavoriteCore f;
    f.Setup_Favorites_Database();

    // Firstrun is now finished, allow the program to load normally by reading new config.
    LoadConfig(true);
}


//##################################################################################
// Load the current raw config data from the robojournal.ini file and buffer it.
void SettingsManager::LoadConfig(bool startup){
    using namespace std;

    bool reload=false;

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";

    // construct a file object where the config file is supposed to be.
    QFile config(config_path);

    if(startup)
        cout << "OUTPUT: Searching for config file.............................";

    // if config file exists, read its contents
    if(config.exists()){

            if(startup)
        cout << ok_param << endl;

#ifdef _WIN32
        // Use backslashes to separate dirs on Windows.
        config_path=QDir::toNativeSeparators(config_path);
#endif

        //cout << "OUTPUT: Config file found: "<< config_path.toStdString()  << endl;
        QSettings settings(config_path,QSettings::IniFormat);
        if(startup)
            cout << "OUTPUT: Buffering data from config file.......................";

        Buffer::toolbar_pos = settings.value("Behavior/toolbar_location").toInt();
        Buffer::allowroot = settings.value("Behavior/allow_root_login").toBool();
        Buffer::allentries = settings.value("Behavior/show_all_entries").toBool();
        Buffer::alwaysusedefaults = settings.value("Behavior/always_use_defaults").toBool();
        Buffer::backend = settings.value("Backend/db_type").toString();
        Buffer::showwarnings = settings.value("Behavior/show_warnings").toBool();
        Buffer::entryrange = settings.value("Backend/entry_range").toInt();
        Buffer::defaulthost = settings.value("Backend/default_host").toString();
        Buffer::defaultdatabase = settings.value("Backend/default_db").toString();
        Buffer::defaultuser = settings.value("Backend/default_user").toString();
        Buffer::databaseport = settings.value("Backend/port").toInt();
        Buffer::sortbyday = settings.value("Behavior/sort_by_day").toBool();
        Buffer::alternate_rows = settings.value("Behavior/alternate_colors").toBool();
        Buffer::use_background = settings.value("Appearance/use_background").toBool();
        Buffer::use_custom_theme = settings.value("Appearance/use_theme").toBool();
        Buffer::tile_bg = settings.value("Appearance/tile_background").toBool();
        Buffer::set_tree_background = settings.value("Appearance/background_tree").toBool();
        Buffer::text_hexcolor = settings.value("Appearance/font_color").toString();
        Buffer::font_face = settings.value("Appearance/font_face").toString();
        Buffer::entry_node_icon = settings.value("Appearance/entry_node_icon").toInt();
        Buffer::show_spell_errors_by_default = settings.value("Behavior/default_show_errors").toBool();
        Buffer::editor_geometry=settings.value("Appearance/editor_size").toSize();

        if(Buffer::editor_geometry.isEmpty()){
            QSize *y=new QSize(640,480);
            Buffer::editor_geometry=*y;

            settings.beginGroup("Appearance");
            settings.setValue("editor_size", *y);
            settings.endGroup();

            reload=true;
        }


        Buffer::config_geometry=settings.value("Appearance/config_size").toSize();

        if(Buffer::config_geometry.isEmpty()){
            QSize *s=new QSize(575,525);
            Buffer::config_geometry=*s;

            settings.beginGroup("Appearance");
            settings.setValue("config_size", *s);
            settings.endGroup();

            reload=true;
        }


        Buffer::preview_geometry=settings.value("Appearance/preview_size").toSize();
        Buffer::mainwindow_geometry=settings.value("Appearance/mainwindow_size").toByteArray();


        Buffer::use_spellcheck = settings.value("Behavior/use_spellcheck").toBool();
        Buffer::current_dictionary= settings.value("Behavior/spellcheck_dictionary").toString();
        Buffer::current_dictionary_aff = settings.value("Behavior/spellcheck_dictionary_aff").toString();
        Buffer::system_dic=settings.value("Behavior/system_dictionaries").toBool();

        int font_value=settings.value("Appearance/font_size").toInt();

        switch(font_value){

        case 0:
            Buffer::font_size="8";
            break;

        case 1:
            Buffer::font_size="9";
            break;

        case 2:
            Buffer::font_size="10";
            break;

        case 3:
            Buffer::font_size="11";
            break;

        case 4:
            Buffer::font_size="12";
            break;

        case 5:
            Buffer::font_size="14";
            break;

        case 6:
            Buffer::font_size="16";
            break;

        case 7:
            Buffer::font_size="18";
            break;

        case 8:
            Buffer::font_size="20";
            break;

        case 9:
            Buffer::font_size="22";
            break;

        case 10:
            Buffer::font_size="24";
            break;

        case 11:
            Buffer::font_size="26";
            break;

        }

        Buffer::keep_time = settings.value("Behavior/use_time").toBool();
        Buffer::show_title = settings.value("Behavior/show_title").toBool();
        Buffer::use_24_hour = settings.value("Behavior/use_24_hour_clock").toBool();
        Buffer::date_format = settings.value("Behavior/set_date_format").toInt();
        Buffer::background_image = settings.value("Appearance/background_image").toString();
        Buffer::show_icon_labels = settings.value("Behavior/enable_toolbar_button_text").toBool();
        Buffer::autoload = settings.value("Behavior/autoload_recent_entry").toBool();
        Buffer::SSL = settings.value("Behavior/ssl_support").toBool();
        Buffer::use_indicator = settings.value("Behavior/display_year_indicator").toBool();
        Buffer::is_male = settings.value("User/user_gender_male").toBool();
        Buffer::full_name = settings.value("User/user_full_name").toString();
        Buffer::use_full_name= settings.value("User/use_full_name").toBool();


        // 0.4 specific
        Buffer::use_rounded_corners = settings.value("Export/rounded_corners").toBool();
        Buffer::use_system_colors = settings.value("Export/system_colors").toBool();
        Buffer::include_tags_in_export = settings.value("Export/include_tags").toBool();
        Buffer::header_font = settings.value("Export/header_font").toString();
        Buffer::body_font = settings.value("Export/body_font").toString();
        Buffer::header_use_em = settings.value("Export/header_use_em").toBool();
        Buffer::body_use_em = settings.value("Export/body_use_em").toBool();
        Buffer::header_font_size = settings.value("Export/header_font_size").toString();
        Buffer::body_font_size = settings.value("Export/body_font_size").toString();
        Buffer::highlight_color = settings.value("Appearance/highlight_color").toInt();
        Buffer::bg_is_fixed = settings.value("Behavior/background_fixed").toBool();
        Buffer::use_highlight = settings.value("Behavior/use_highlights").toBool();
        Buffer::trim_whitespace = settings.value("Behavior/trim_whitespace").toBool();
        Buffer::use_smart_quotes = settings.value("Behavior/smart_quotes").toBool();
        Buffer::use_html_hyphens = settings.value("Behavior/html_hyphens").toBool();
        Buffer::datebox_override = settings.value("Appearance/datebox_override").toBool();
        Buffer::use_custom_theme_editor=settings.value("Appearance/use_theme_editor").toBool();
        Buffer::use_dow=settings.value("Behavior/use_dow").toBool();
        Buffer::use_misc_processing=settings.value("Behavior/misc_processing").toBool();


        // 0.4.1 specific
        Buffer::name_in_titlebar=settings.value("Behavior/name_in_titlebar").toBool();
        Buffer::show_untagged_reminder=settings.value("Behavior/show_untagged_reminder").toBool();

        // 0.4.2 ( backported to 0.4.2 on 9/13/13)
        Buffer::mw_splitter_size=settings.value("Behavior/mw_splitter_position").toByteArray(); // added 6/21/13

        // 0.5 options (6/5/13)
        Buffer::use_my_journals=settings.value("SQLite/use_my_journals").toBool();
        Buffer::sqlite_default=settings.value("SQLite/sqlite_default").toString();
        Buffer::sqlite_favorites=settings.value("SQLite/sqlite_favorites").toStringList();
        Buffer::mysqldump_path_win=settings.value("Behavior/mysqldump_path_win").toString(); // added 9/2/13
        Buffer::open_editor = settings.value("Behavior/open_editor").toBool(); // Added 7/4/14.
        Buffer::helpdoc= settings.value("Behavior/help_page").toString(); // Added 7/20/14.

        if(reload){
            LoadConfig(false);
        }

        if(startup){
            cout << ok_param << endl;

            cout << "OUTPUT: Stage 1 completed, proceeding to Stage 2." << endl;
        }
    }

    // if config doesn't exist, rebuild it.
    else{
        if(startup){
            cout << fail_param << endl;
            cout << "OUTPUT: Starting first run mode to replace missing configuration." << endl;
        }

        // give the journal creator a way to know its the first run
        Buffer::firstrun=true;

        // Launch the firstrun tool
        FirstRun f;
        f.exec();
    }
}

//##################################################################################
// Update the settings with data from the Newconfig class
void SettingsManager::UpdateConfig(){

    QString config_path=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "robojournal.ini";
    QSettings settings(config_path,QSettings::IniFormat);

    settings.clear();

    // sort new data into groups and add it, this is much better than the old way.
    settings.beginGroup("Backend");
    settings.setValue("default_host", Newconfig::new_default_host.trimmed());
    settings.setValue("default_db", Newconfig::new_default_db.trimmed());
    settings.setValue("default_user", Newconfig::new_default_user.trimmed());
    settings.setValue("db_type", Newconfig::new_dbtype.trimmed());
    settings.setValue("port", Newconfig::new_default_port.trimmed());
    settings.setValue("entry_range", Newconfig::new_entry_range);
    settings.endGroup();

    // New SQLite options for RoboJournal 0.5
    settings.beginGroup("SQLite");
    settings.setValue("use_my_journals", Newconfig::new_use_my_journals);
    settings.setValue("sqlite_favorites", Newconfig::new_sqlite_favorites);
    settings.setValue("sqlite_default", Newconfig::new_sqlite_default);
    settings.endGroup();


    settings.beginGroup("Behavior");
    settings.setValue("toolbar_location", Newconfig::new_toolbar_pos);
    settings.setValue("allow_root_login", Newconfig::new_allow_root);
    settings.setValue("show_all_entries", Newconfig::new_show_all_entries);
    settings.setValue("always_use_defaults", Newconfig::new_always_use_defaults);
    settings.setValue("show_warnings", Newconfig::new_show_confirm);
    settings.setValue("sort_by_day", Newconfig::new_sort_by_day);
    settings.setValue("alternate_colors", Newconfig::new_alternate_rows);
    settings.setValue("use_time", Newconfig::new_record_time);
    settings.setValue("show_title", Newconfig::new_show_title);
    settings.setValue("use_24_hour_clock", Newconfig::new_24_hr);
    settings.setValue("set_date_format", Newconfig::new_date_mode);
    settings.setValue("enable_rich_text", Newconfig::new_rich_text);
    settings.setValue("use_dow", Newconfig::new_use_dow);
    settings.setValue("enable_toolbar_button_text", Newconfig::new_icon_labels);
    settings.setValue("autoload_recent_entry", Newconfig::new_autoload);
    settings.setValue("ssl_support", Newconfig::new_SSL);
    settings.setValue("display_year_indicator", Newconfig::new_indicator);
    settings.setValue("use_highlights", Newconfig::new_use_highlights);
    settings.setValue("trim_whitespace", Newconfig::new_trim_whitespace);
    settings.setValue("smart_quotes", Newconfig::new_use_smart_quotes);
    settings.setValue("html_hyphens", Newconfig::new_use_html_hyphens);
    settings.setValue("use_spellcheck", Newconfig::new_use_spellcheck);
    settings.setValue("default_show_errors", Newconfig::new_show_spell_errors_by_default);
    settings.setValue("spellcheck_dictionary", Newconfig::new_current_dictionary);
    settings.setValue("spellcheck_dictionary_aff",Newconfig::new_current_dictionary_aff);
    settings.setValue("misc_processing",Newconfig::new_use_misc_processing);
    settings.setValue("name_in_titlebar", Newconfig::new_name_in_titlebar);
    settings.setValue("show_untagged_reminder", Newconfig::new_show_untagged_reminder);
    settings.setValue("system_dictionaries", Newconfig::new_system_dic);
    settings.setValue("open_editor", Newconfig::new_open_editor);
    settings.endGroup();

    settings.beginGroup("Appearance");
    settings.setValue("background_image", Newconfig::new_background_image);
    settings.setValue("use_background", Newconfig::new_use_background);
    settings.setValue("use_theme", Newconfig::new_use_custom_theme);
    settings.setValue("use_theme_editor", Newconfig::new_use_custom_theme_editor);
    settings.setValue("tile_background", Newconfig::new_tile_bg);
    settings.setValue("background_tree", Newconfig::new_use_background_in_tree);
    settings.setValue("font_color", Newconfig::new_text_hexcolor);
    settings.setValue("font_face", Newconfig::new_font_face);
    settings.setValue("font_size", Newconfig::new_font_size);
    settings.setValue("datebox_override", Newconfig::new_datebox_override);
    settings.setValue("highlight_color", Newconfig::new_highlight_color);
    settings.setValue("entry_node_icon", Newconfig::new_entry_node_icon);
    settings.endGroup();

    settings.beginGroup("Export");
    settings.setValue("rounded_corners", Newconfig::new_use_rounded_corners);
    settings.setValue("system_colors", Newconfig::new_use_system_colors);
    settings.setValue("include_tags", Newconfig::new_include_tags_in_export);
    settings.setValue("header_font", Newconfig::new_header_font);
    settings.setValue("body_font", Newconfig::new_body_font);
    settings.setValue("header_use_em", Newconfig::new_header_use_em);
    settings.setValue("body_use_em", Newconfig::new_body_use_em);
    settings.setValue("header_font_size", Newconfig::new_header_font_size);
    settings.setValue("body_font_size", Newconfig::new_body_font_size);
    settings.endGroup();

    settings.beginGroup("User");
    settings.setValue("user_gender_male", Newconfig::new_gender_is_male);
    settings.setValue("user_full_name", Newconfig::new_full_name);
    settings.setValue("use_full_name", Newconfig::new_use_full_name);
    settings.endGroup();

    // save the settings
    settings.sync();
}
