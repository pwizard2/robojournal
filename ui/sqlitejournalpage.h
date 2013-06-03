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
    QString ProcessFilename(QString raw);


private slots:
      void on_DatabaseName_textChanged(const QString &arg1);
};

#endif // SQLITEJOURNALPAGE_H
