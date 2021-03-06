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

    Class description/purpose (7/4/13), --Will Kraft:
    The OutputPane class holds the current entry AND its tags. This is
    designed to work with the completely redesigned Rich Text entry system.
    I originally planned to implement this and the rich text entry system
    in version 0.5 but due to the sheer scope of the necessary changes AND
    the requisite months of debugging it looks like it's going to have to
    wait until 0.6.
*/

#ifndef OUTPUTPANE_H
#define OUTPUTPANE_H

#include <QWidget>

namespace Ui {
    class OutputPane;
}

class OutputPane : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPane(QWidget *parent = 0);
    ~OutputPane();

private:
    Ui::OutputPane *ui;
};

#endif // OUTPUTPANE_H
