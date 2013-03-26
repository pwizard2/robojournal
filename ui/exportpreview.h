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


#ifndef EXPORTPREVIEW_H
#define EXPORTPREVIEW_H
#include <QDialog>


namespace Ui {
    class ExportPreview;
}

class ExportPreview : public QDialog
{
    Q_OBJECT

public:
    explicit ExportPreview(QWidget *parent = 0);
    ~ExportPreview();



    static QString header_font;
    static QString body_font;
    static QString header_size;
    static QString body_size;

    static bool use_rounded;
    static bool use_system_colors;
    static bool use_em_header;
    static bool use_em_body;

private slots:


private:
    Ui::ExportPreview *ui;


    QString PrepareHTML();


};

#endif // EXPORTPREVIEW_H
