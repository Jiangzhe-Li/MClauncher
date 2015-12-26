#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = 0);
    ~InfoDialog();

    void setInfo(QString , QString , QString );

private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
