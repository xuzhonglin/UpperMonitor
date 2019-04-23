#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->cmdId->setVisible(false);//隐藏cmdId
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::reject()
{
    delete this;
}

void Dialog::accept()
{
    QString cmdName=ui->cmdName->text();
    QString cmdContent=ui->cmdContent->toPlainText();
    int cmdId=ui->cmdId->text().toInt();
    emit updateCmd(cmdId,cmdName,cmdContent);
    delete this;
}

void Dialog::cmd_modified(int cmdId,QString cmdName,QString cmdContent)
{
    ui->cmdId->setText(QString::number(cmdId));
    ui->cmdName->setText(cmdName);
    ui->cmdContent->setText(cmdContent);
}
