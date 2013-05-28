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
    
private:
    Ui::SQLiteJournalPage *ui;
};

#endif // SQLITEJOURNALPAGE_H
