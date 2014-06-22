/*
    This file is part of RoboJournal.
    Copyright (c) 2014 by Will Kraft <pwizard@gmail.com>.


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



#ifndef HELPVIEWER_H
#define HELPVIEWER_H

#include <QDialog>

namespace Ui {
class HelpViewer;
}

class HelpViewer : public QDialog
{
    Q_OBJECT

public:
    explicit HelpViewer(QWidget *parent = 0);
    ~HelpViewer();

private slots:
    void on_BackButton_clicked();

    void on_HomeButton_clicked();

    void on_forwardButton_clicked();

    void on_ChangelogButton_clicked();

private:
    Ui::HelpViewer *ui;
    void LoadDoc();
    void PrimaryConfig();
};

#endif // HELPVIEWER_H
