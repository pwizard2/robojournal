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

#ifndef TAGGER_H
#define TAGGER_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class Tagger;
}

class Tagger : public QDialog
{
    Q_OBJECT
    
public:
    explicit Tagger(QWidget *parent = 0);
    ~Tagger();

    static QString id_num;
    static QString title;
    


private slots:
    void on_RemoveTag_clicked();

    void on_TagList_itemSelectionChanged();

    void on_AddTag_clicked();

    void on_TagChooser_editTextChanged(const QString &arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_NewTag_clicked();

    void on_TagChooser_currentIndexChanged();

private:
    Ui::Tagger *ui;

    void AddTag(QString newtag);
    void DeleteTag();
    void TagAggregator();
    void SaveTags();
    void LoadTags();
    void AddTagToList();
    QString ExportTagList();


};

#endif // TAGGER_H
