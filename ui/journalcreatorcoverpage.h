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

    6/23/13: This class is just a cover page for the NewJournalCreator class.
    Its purpose is to provide info about each type of backend RoboJournal can use.

*/

#ifndef JOURNALCREATORCOVERPAGE_H
#define JOURNALCREATORCOVERPAGE_H

#include <QWidget>

namespace Ui {
class JournalCreatorCoverPage;
}

class JournalCreatorCoverPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit JournalCreatorCoverPage(QWidget *parent = 0);
    ~JournalCreatorCoverPage();
    
private:
    Ui::JournalCreatorCoverPage *ui;
};

#endif // JOURNALCREATORCOVERPAGE_H
