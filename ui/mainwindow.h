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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QCloseEvent>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString Connection;
    QString hostname;
    QString username;
    QString password;
    QString database;


    QList<QString> IDList;

    static QWidget* mw;


private slots:
    void on_WriteButton_clicked();
    void on_SearchButton_clicked();
    void on_ConnectButton_clicked();
    void on_DisconnectButton_clicked();
    void on_ConfigButton_clicked();
    void on_actionPreferences_2_triggered();
    void on_actionConnect_triggered();
    void on_actionToolbar_triggered(bool checked);
    void on_actionCopy_triggered();
    void on_actionSelect_all_triggered();
    void on_actionMain_toolbar_triggered(bool checked);
    void on_actionLeft_default_triggered();
    void on_actionTop_triggered();
    void on_actionRight_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_RoboJournal_triggered();
    void on_TodayButton_clicked();
    void on_LastEntry_clicked();
    void on_actionLatest_Entry_triggered();
    void on_actionDisconnect_triggered();
    void on_EntryList_itemClicked(QTreeWidgetItem *item);
    void on_NextEntry_clicked();
    void on_actionPrevious_Entry_triggered();
    void on_actionNext_Entry_triggered();
    void on_actionEdit_Selected_Entry_triggered();
    void on_actionSetup_Wizard_2_triggered();
    void on_actionPrint_triggered();

    void on_DeleteEntry_clicked();

    void on_actionDelete_Current_Entry_triggered();

    void on_actionWrite_triggered();

    void on_actionHelp_file_triggered();

    void on_EditEntryButton_clicked();

    void on_Tag_clicked();

    void on_actionManage_Tags_2_triggered();


    void on_actionSelect_Default_Journal_triggered();

    void on_ExportEntry_clicked();

    void on_actionExport_Entry_triggered();

    void on_actionEntry_List_toggled(bool arg1);

    void on_SearchFilter_currentIndexChanged(const QString &arg1);

    void on_ClearButton_clicked();

    void on_actionJournal_Creator_triggered();

    void on_tabWidget_currentChanged(int index);



    void on_WholeWords_clicked();

    void on_SearchList_currentItemChanged(QTreeWidgetItem *current);

    void on_actionJournal_Selector_triggered();

    void on_actionRestore_Splitter_Position_triggered();

private:
    Ui::MainWindow *ui;
    void PrimaryConfig();
    void CreateTree();
    void ChangeSelection(QString rowid, QString rowtitle);
    void ShowHelp();
    void Modify();
    void Connect();
    void Disconnect();
    void Preferences();
    void ShowCredits();
    void Write();
    void MostRecent();
    void GetPrevious();
    void GetEntry(QString id, bool fromSearch);
    void GetAdjacent(int direction);
    void DeleteSelectedEntry();
    void TotalEntryCount(int totalcount);
    void Decorate_GUI();
    void Print();
    void ExportEntry();
    bool SaveEntry();
    void UpgradeJournal();
    QString GetLongMonth(int month);
    void closeEvent(QCloseEvent *event);
    QString FormatTags(QString tags, QString color1, QString color2);
    void Tag();
    void GetTagList();
    void SearchDatabase();
    QStringList HighlightResults(QString entry, QString title, QString term, int index);
    void SwitchTab(int index);
    void ClearSearchResults();


    QString InvertColor(QString textcolor); // returns hex color inverse of input text color

    QString CreateStyleSheet(bool for_entrylist, bool is_TextEdit); // create stylesheet for Decorate_GUI function

    void UpdateSelectedEntry(QString id);
    QString CurrentID; // ID of currently selected entry

    QList<QString> hilite_list; // list that contains entries and their index pos as they are added

    QString Record; // contains the current record number (row id)

    int num_records; // this int holds the number of records. This is used for several functions.

    QString Global_Datestamp; // global date used for EntryExporter. When the user fetches a record, this value should hold the date for that entry.


    QString current_entry_date; // date for entry exporter
    QString current_entry_body; //body text for extry exporter
    QString current_entry_time; // timestamp for entry exporter

    //QTreeWidgetItem current;

    // returns the current selected tree item. This function is called in the back-and-forth buttons.
    void HighlightCurrentSelection(QString CurrentID);


    // should be true if db connection is on, false if it is not
    bool ConnectionActive;

    // magic sizing algorithm for splitter:
    int splittersize;

    // keep track if search is active or not
    bool has_search_results;

    //bool keep track of whether we used the editor or not.
    bool launched_editor;
    bool launched_config;

};

#endif // MAINWINDOW_H
