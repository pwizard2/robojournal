#ifndef MYSQLJOURNALPAGE_H
#define MYSQLJOURNALPAGE_H

#include <QWidget>


namespace Ui {
class MySQLJournalPage;
}

class MySQLJournalPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit MySQLJournalPage(QWidget *parent = 0);
    ~MySQLJournalPage();

signals:
    void unlockOK();
    void unlockNotOK();

public slots:
    void ClearForm();

    
private slots:
    void on_Password1_textChanged();

    void on_Password2_textEdited();

private:
    Ui::MySQLJournalPage *ui;
    void PrimaryConfig();
    void PasswordsMatch();

};

#endif // MYSQLJOURNALPAGE_H
