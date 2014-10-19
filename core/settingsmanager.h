/*
    This file is part of RoboJournal.
    Copyright (c) 2015 by Will Kraft <pwizard@gmail.com>.
    

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


#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QByteArray>

class SettingsManager
{

public:
    SettingsManager();

    void LoadConfig(bool startup);
    void UpdateConfig();
    void NewConfig(QString host,QString db_name, QString port, QString newuser);
    QString FullName();
    bool GenderCheck();
    void InstallDictionaries();
    void SaveEditorSize(QSize geo);
    void SaveConfigSize(QSize geo);
    void SavePreviewSize(QSize geo);
    void SaveMainWindowSize(QByteArray geo);
    void SaveNagPreferences();
    void SaveSplitterPos(QByteArray value);
    void Save_Mysqldump_Path(QString path);
    void Save_HelpDoc(QString path);


private:
    //extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
    std::string ok_param;
    std::string fail_param;
};

#endif // SETTINGSMANAGER_H
