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

#ifndef CONFIGURATIONJOURNAL_H
#define CONFIGURATIONJOURNAL_H

#include <QWidget>

namespace Ui {
    class ConfigurationJournal;
}

class ConfigurationJournal : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigurationJournal(QWidget *parent = 0);
    ~ConfigurationJournal();
    void GetChanges();

private:
    Ui::ConfigurationJournal *ui;
    void PrimaryConfig();
    void PopulateForm();

private slots:
      void on_ShowAllEntries_clicked(bool checked);
      void on_Highlight_clicked(bool checked);
      void on_FullName_textChanged(const QString &arg1);
};

#endif // CONFIGURATIONJOURNAL_H
