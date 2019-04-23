#ifndef UPPERMAIN_H
#define UPPERMAIN_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QSettings>
#include <QCloseEvent>
#include <qmessagebox.h>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QDebug>
#include <QFileDialog>
#include <QDateTime>
#include <QTimer>

#include "custom/dialog.h"
#include "custom/help.h"
#include "component/ClickLabel.h"
#include "component/DoubleClickButton.h"


#define COM_EVENT_TIMEOUT 1000
#define RX_EVENT_TIMEOUT 100

const QString APP_NAME="UpperMonitor";

namespace Ui {
class UpperMain;
}

class UpperMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit UpperMain(QWidget *parent = nullptr);
    ~UpperMain();


signals:
    void modifyCmd(int cmdId,QString cmdName,QString cmdContent);


private:
    Ui::UpperMain *ui;
    Dialog *dialog;
    QSignalMapper *mymapper;
    QSerialPortInfo *serialinfo;
    QSerialPort *serial;
    QStringListModel *Model;
    QStandardItemModel *ItemModel;
    QTimer *com_timer;
    QTimer *rx_listen_timer;
    QTimer *send_event_timer;

    help *h;
    QString cmd[6];//定义cmd容器
    DoubleClickedButton *cmdbtn[6];
    QMap<QString,QSerialPortInfo> serialInfoList;
    QSerialPortInfo currentSerial;

    int rxBitsCounter;
    int txBitsCounter;
    bool comOpenState;
    bool rxPauseState;

    void closeEvent(QCloseEvent *event);
    void initComponent(void);
    void initConf(void);
    void saveConf(void);
    void initPara(void);

    QByteArray convertStringToHex(QString &hexStr);
    char convertCharToHex(char ch);
    bool includeChinese(QString str);

    union HexToFloat{
        char hex[4];
        float value;
    }tem,hum;

private slots:
    void cmd_btn_doubleclicked(int cmdId);
    void cmd_btn_singleclicked(int cmdId);
    void cmd_updated(int cmdId,QString cmdName,QString cmdContent);
    void rx_data_save_clicked();
    void com_open_clicked();
    void tx_send_clicked();
    void rx_read_listen();
    void timer_event();
    void timer_send(bool state);

};

#endif // UPPERMAIN_H
