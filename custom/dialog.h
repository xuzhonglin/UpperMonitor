#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

signals:
    void updateCmd(int cmdId,QString cmdName,QString cmdContent);

public slots:
    void cmd_modified(int cmdId,QString cmdName,QString cmdContent);


private:
    Ui::Dialog *ui;

private slots:
    void reject();
    void accept();
};

#endif // DIALOG_H
