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


#ifndef ENTRYEXPORTER_H
#define ENTRYEXPORTER_H

#include <QDialog>
#include <QStringList>


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
    static QString timestamp;
    void UpdateValues(QString new_title, QString new_date, QString new_body, QString new_timestamp);

    
private slots:
    void on_BrowseButton_clicked();

    void on_PlainText_clicked();

    void on_HTML_clicked();

    void on_IncludeDate_clicked();

    void on_IncludeJournalName_clicked();

    void on_buttonBox_accepted();

    void on_HTML_2_clicked();

    void on_PlainText_2_clicked();


    void on_IncludeExportDate_clicked();

private:
    Ui::EntryExporter *ui;
    void PrimaryConfig();
    void SetName();
    void Set_Mass_Name();
    void Browse();
    void Do_Export();
    void Mass_Export();
    void Validate();
    QStringList Do_Word_Wrap(QString body);
    void SetupCSS();

    QString body_font;
    QString header_font;
    QString body_font_size;
    QString header_font_size;
    QString datebox_bgcolor;
    QString datebox_color;

    QString ProcessEntryName(bool use_journal, bool use_date);
};

#endif // ENTRYEXPORTER_H
