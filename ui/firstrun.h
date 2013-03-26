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

#ifndef FIRSTRUN_H
#define FIRSTRUN_H

#include <QDialog>

namespace Ui {
    class FirstRun;
}

class FirstRun : public QDialog
{
    Q_OBJECT

public:
    explicit FirstRun(QWidget *parent = 0);
    ~FirstRun();


private slots:





    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::FirstRun *ui;

    void Launcher();

};

#endif // FIRSTRUN_H
