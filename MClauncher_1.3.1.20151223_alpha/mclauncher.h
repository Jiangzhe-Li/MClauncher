#ifndef MCLAUNCHER_H
#define MCLAUNCHER_H

#include <QMainWindow>

#include <QProcess>
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include "settingdialog.h"
#include "MClauncherCore.h"
#include <QDialog>
#include <QLabel>
#include <QLayout>

namespace Ui {
    class MClauncher;
}

class MClauncher : public QMainWindow
{
    Q_OBJECT

public:
    explicit MClauncher(QWidget *parent = 0);
    ~MClauncher();

private:
    Ui::MClauncher *ui;

    QPoint windowPos,mousePos,dPos;// Û±ÍŒª÷√…Í√˜

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void settingDialogSlot();
    void startGameSlot();
};

#endif // MCLAUNCHER_H
