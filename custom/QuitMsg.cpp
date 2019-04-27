#include "QuitMsg.h"
#include "ui_quitmsg.h"

QuitMsg::QuitMsg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuitMsg)
{
    ui->setupUi(this);
}

QuitMsg::~QuitMsg()
{
    delete ui;
}

void QuitMsg::accept()
{
    bool save_state=ui->chk_save_conf->checkState();
    emit confirmSave(save_state);
    QDialog::accept();
}

void QuitMsg::reject()
{
    QDialog::reject();
}
