/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    

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

    Class Description / Purpose:
    The EntryExporter class used to be responsible for converting database
    entries to HTML or plain text. As of RoboJournal 0.5, the EntryExporter
    is just a container for the ExportSingle/ExportMulti pages that do
    the actual export work. --Will Kraft (7/30/13).

  */


#ifndef ENTRYEXPORTER_H
#define ENTRYEXPORTER_H

#include <QDialog>
#include <QStringList>
#include "ui/exportmulti.h"
#include "ui/exportsingle.h"
#include "ui/exportcreatedump.h"
#include <QStackedWidget>


namespace Ui {
class EntryExporter;
}

class EntryExporter : public QDialog
{
    Q_OBJECT
    
public:
    explicit EntryExporter(QWidget *parent = 0);
    ~EntryExporter();

    static QString title; // title of this entry
    static QString body; // body text of this entry
    static QString date; //date of this entry used in filename
    static QString id; // id of this entry (added for improved export function in 0.5).
    static QString timestamp;
    void UpdateValues(QString new_title, QString new_date, QString new_body, QString new_timestamp);

public slots:
    void lockOKButton();
    void unlockOKButton();

private slots:

    void on_Menu_currentRowChanged(int currentRow);

private:
    Ui::EntryExporter *ui;
    void PrimaryConfig();

    bool Validate();

    QString body_font;
    QString header_font;
    QString body_font_size;
    QString header_font_size;
    QString datebox_bgcolor;
    QString datebox_color;

    QString ProcessEntryName(bool use_journal, bool use_date);

    QStackedWidget *stack;
    ExportSingle *single;
    ExportMulti *multi;
    ExportCreateDump *dump;

    QPushButton* export_button;

    void accept();

};

#endif // ENTRYEXPORTER_H
