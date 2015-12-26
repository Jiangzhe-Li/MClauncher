#include "infodialog.h"
#include "ui_infodialog.h"

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::setInfo(QString info1, QString info2, QString info3){
    ui->infoLabel1->setText(info1);
    ui->infoLabel2->setText(info2);
    ui->infoLabel3->setText(info3);
}
