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

    The CustomWords class provides a user interface for managing user-defined dictionary words.
    RoboJournal stores the list of these words in ~/.robojournal. --Will Kraft (11/3/13).
*/

#ifndef CUSTOMWORDS_H
#define CUSTOMWORDS_H

#include <QDialog>

namespace Ui {
    class CustomWords;
}

class CustomWords : public QDialog
{
    Q_OBJECT

public:
    explicit CustomWords(QWidget *parent = 0);
    ~CustomWords();

private:
    Ui::CustomWords *ui;
    void PrimaryConfig();
    QStringList Load_Words();
    QString file_path;
};

#endif // CUSTOMWORDS_H
