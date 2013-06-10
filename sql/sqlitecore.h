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

#ifndef SQLITECORE_H
#define SQLITECORE_H


#include <QtSql/QSqlDatabase>


class SQLiteCore
{
public:
    SQLiteCore();
    bool Connect();
    bool Disconnect();
    bool AddEntry();
    bool RemoveEntry();
    bool CreateDB(QString dbname);

    static QSqlDatabase db;


};

#endif // SQLITECORE_H
