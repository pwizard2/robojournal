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
