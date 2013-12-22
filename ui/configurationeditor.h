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

#ifndef CONFIGURATIONEDITOR_H
#define CONFIGURATIONEDITOR_H

#include <QWidget>

namespace Ui {
class ConfigurationEditor;
}

class ConfigurationEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConfigurationEditor(QWidget *parent = 0);
    ~ConfigurationEditor();
    void GetChanges();
    
private:
    Ui::ConfigurationEditor *ui;
    void PopulateForm();
    QString aff_file;
    QString Find_AFF_File(QString dict);
    QStringList Scan_For_System_Dictionaries();

private slots:
    void on_BrowseButton_clicked();
    void on_UseSpellCheck_clicked(bool checked);
    void on_SystemLevelDic_toggled(bool checked);
    void on_ManageUDWords_clicked();
    void on_Dictionary_currentIndexChanged(const QString &arg1);
};

#endif // CONFIGURATIONEDITOR_H
