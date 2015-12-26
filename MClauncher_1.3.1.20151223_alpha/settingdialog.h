#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QMouseEvent>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcessEnvironment>
#include "infodialog.h"


namespace Ui {
    class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private:
    Ui::SettingDialog *ui;

    QPoint windowPos,mousePos,dPos;// Û±ÍŒª÷√…Í√˜

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void autoRearchJavaPathSlot();
    void browseSlot();
    void saveAndExitSlot();
    void aboutSoftSlot();
};

#endif // SETTINGDIALOG_H
