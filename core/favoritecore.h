/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
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

    Class description/purpose (7/18/13) --Will Kraft:
    The FavoriteCore class is responsible for managing the list of
    favorite databases. If the user has several databases that are used
    on a regular basis, the favorites system allows easy switching.
    In the past, it was necessary to redefine the default or enter
    the database name manually at the start of each connection. This class
    should be included in all other classes that interact with the
    favorites system.
*/

#ifndef FAVORITECORE_H
#define FAVORITECORE_H

#include <QStringList>
#include <QString>
#include <QSqlDatabase>
#include <QList>

class FavoriteCore
{

public:

    FavoriteCore();
    void setFavorite(QString id, bool favorite);
    QList<QStringList> getKnownJournals();
    void Setup_Favorites_Database();
    void Add_to_DB(QString database, QString user, QString host);
    void Remove_from_DB(QString id);
    void init();
    QStringList GetFavorites(QString host);
    QStringList GetHosts();

private:

    QString favorite_db_path;
    QSqlDatabase favorite_db;

};

#endif // FAVORITECORE_H
