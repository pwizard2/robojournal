#ifndef EXPORTCREATEDUMP_H
#define EXPORTCREATEDUMP_H

#include <QWidget>

namespace Ui {
class ExportCreateDump;
}

class ExportCreateDump : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExportCreateDump(QWidget *parent = 0);
    ~ExportCreateDump();
    
private:
    Ui::ExportCreateDump *ui;
};

#endif // EXPORTCREATEDUMP_H
