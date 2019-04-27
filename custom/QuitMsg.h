#ifndef QUITMSG_H
#define QUITMSG_H

#include <QDialog>

namespace Ui {
class QuitMsg;
}

class QuitMsg : public QDialog
{
    Q_OBJECT

public:
    explicit QuitMsg(QWidget *parent = nullptr);
    ~QuitMsg();

signals:
    void confirmSave(bool state);

private:
    Ui::QuitMsg *ui;

private slots:
    void accept();
    void reject();
};

#endif // QUITMSG_H
