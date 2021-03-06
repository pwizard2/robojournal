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

    --Will Kraft, 6/11/13. New for Version 0.5.
    This class contains tag-related functions that are "shared" between many classes.
    When one of these functions is needed, the app calls the method from this class
    instead of handling everything internally. It made more sense to design the app
    this way instead of having redundant code.
  */

#ifndef TAGGINGSHARED_H
#define TAGGINGSHARED_H

#include <QStringList>
#include <QString>


class TaggingShared
{
public:
    TaggingShared();

    QStringList TagAggregator();
    QStringList FetchTags(QString id);
    void SaveTags(QString tags, QString id);
    QString DefineTag(QStringList ExistingTags);

};

#endif // TAGGINGSHARED_H
