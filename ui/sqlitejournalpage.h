#ifndef SQLITEJOURNALPAGE_H
#define SQLITEJOURNALPAGE_H

#include <QWidget>

namespace Ui {
class SQLiteJournalPage;
}

class SQLiteJournalPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit SQLiteJournalPage(QWidget *parent = 0);
    ~SQLiteJournalPage();


public slots:
    void ClearForm();

signals:
    void unlockOK();
    void unlockNotOK();
    
private:
    Ui::SQLiteJournalPage *ui;
    void PrimaryConfig();
    void Browse(QString startpath);
    bool FilenameValid(QString filename);
    void ProcessFilename(QString filename, bool valid);


private slots:
      void on_DatabaseName_textChanged(const QString &arg1);
      void on_BrowseButton_clicked();
};

#endif // SQLITEJOURNALPAGE_H
