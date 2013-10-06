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

    Class description / purpose:
    The ExportCreateDump class allows the user to create backups of
    the current database by invoking mysqldump. The ExportCreateDump class
    is displayed as a "page" in the EntryExporter container
    class. --Will Kraft (8/3/13).
 */

#ifndef EXPORTCREATEDUMP_H
#define EXPORTCREATEDUMP_H

#include <QWidget>

namespace Ui {
class ExportCreateDump;
}

class ExportCreateDump : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExportCreateDump(QWidget *parent = 0);
    ~ExportCreateDump();
    bool Create_SQL_Dump(QString filename, QString mysqldump_path);
    bool HarvestData();

    static QString export_path;
    static QString mysqldump_exec;
    
private slots:
    void on_AllowCustomName_toggled(bool checked);

    void on_FileBrowse_clicked();

    void on_DumpBrowse_clicked();

    void on_DumpFileName_textChanged(const QString &arg1);

private:
    Ui::ExportCreateDump *ui;
    void PrimaryConfig();
    QString setFilename();

    QString gzip_path;
    QString mysqldump_path;
    void Validate_Dump_File(QString database, QString filename);
    bool gzip_available;

    QString outputBrowse(QString current_dir);
    QString dumpBrowse(QString current_exec);

    bool FilenameValid(QString filename);
    bool Verify_Output_FileName();
    void ProcessFilename(QString filename, bool valid);
};

#endif // EXPORTCREATEDUMP_H
