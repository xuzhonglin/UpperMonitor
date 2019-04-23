#include "uppermain.h"
#include "ui_uppermain.h"



UpperMain::UpperMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpperMain),
    serial(new QSerialPort),
    ItemModel(new QStandardItemModel)
{
    ui->setupUi(this);
    initPara();
    initConf();
    initComponent();//初始化CMD按键,绑定点击事件
}

UpperMain::~UpperMain()
{
    delete ui;
}

void UpperMain::closeEvent(QCloseEvent *event)
{
    bool state=QMessageBox::question(this,"关闭确认","是否确定退出该软件?\t\n你的配置将会被保存！","取消","确定");
    if(state)
    {
        event->accept();
        saveConf();//保存配置
    }else{
        event->ignore();
    }
}


void UpperMain::cmd_btn_doubleclicked(int cmdId)
{
    dialog=new Dialog(this);
    dialog->setWindowTitle("修改 CMD"+QString::number(cmdId+1)+" 命令");
    dialog->setModal(true);
    dialog->setFixedSize(dialog->width(),dialog->height());
    QString cmdName=cmdbtn[cmdId]->text();
    connect(dialog,SIGNAL(updateCmd(int,QString,QString)),this,SLOT(cmd_updated(int,QString,QString)));
    connect(this,SIGNAL(modifyCmd(int,QString,QString)),dialog,SLOT(cmd_modified(int,QString,QString)));
    emit modifyCmd(cmdId,cmdName,cmd[cmdId]);
    dialog->exec();
}

void UpperMain::cmd_btn_singleclicked(int cmdId)
{
    ui->txText->setPlainText(cmd[cmdId]);
}

void UpperMain::rx_data_save_clicked()
{
    QFileDialog fileDialog;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMddhhmmss");
    QString fileName = fileDialog.getSaveFileName(this,tr("文件保存为"),"data_"+current_date,tr("Text File(*.txt)"));
    if(fileName == "")
    {
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败\t"));
        return;
    }
    else
    {
        QTextStream textStream(&file);
        QString str = ui->rxText->toPlainText();
        textStream<<str;
        QMessageBox::information(this,tr("提示"),tr("保存文件成功\t"));
        file.close();
    }
}

void UpperMain::initComponent(void)
{
    for(int i=0;i<6;i++)
    {
        mymapper=new QSignalMapper();
        connect(cmdbtn[i], SIGNAL(doubleClicked()), mymapper, SLOT(map()));
        mymapper->setMapping(cmdbtn[i],i);
        connect(mymapper, SIGNAL(mapped(int)), this, SLOT(cmd_btn_doubleclicked(int)));
    }
    for(int i=0;i<6;i++)
    {
        mymapper=new QSignalMapper();
        connect(cmdbtn[i], SIGNAL(singleClicked()), mymapper, SLOT(map()));
        mymapper->setMapping(cmdbtn[i],i);
        connect(mymapper, SIGNAL(mapped(int)), this, SLOT(cmd_btn_singleclicked(int)));
    }

    connect(ui->rxTxCountClear,&MyLabel::labelClicked,[this]{rxBitsCounter=txBitsCounter=0;ui->rxBitCountNum->setText("----");ui->txBitCountNum->setText("----");});
    connect(ui->rxPauseBtn,&QPushButton::clicked,[this]{rxPauseState=!rxPauseState;ui->rxPauseBtn->setText(rxPauseState?"继续接收":"暂停接收");});
    connect(ui->help,&MyLabel::labelClicked,[this]{h=new help(this);h->show();h->setFixedSize(h->width(),h->height());h->setWindowTitle("帮助");});

    connect(ui->aboutMe,&MyLabel::labelClicked,[this]{QMessageBox::information(this,tr("彩蛋-郑愁予《错误》"), "我打江南走过\n"
                                                                                                      "那等在季节里的容颜如莲花的开落\t\n"
                                                                                                      "东风不来，三月的柳絮不飞\n"
                                                                                                      "你的心如小小的寂寞的城\n"
                                                                                                      "恰若青石的街道向晚\n"
                                                                                                      "跫音不响，三月的春帷不揭\n"
                                                                                                      "你的心是小小的窗扉紧掩\n"
                                                                                                      "我达达的马蹄是美丽的错误\n"
                                                                                                      "我不是归人，是个过客……\n",tr("关闭"));});
    //connect(ui->clearHistory,&QPushButton::clicked,[this]{ItemModel->clear();});
    connect(ui->txTimerCkb,SIGNAL(clicked(bool)),this,SLOT(timer_send(bool)));
    com_timer = new QTimer(this);//每秒读一次
    connect(com_timer, SIGNAL(timeout()), this, SLOT(timer_event()));
    com_timer->start(COM_EVENT_TIMEOUT);
}


void UpperMain::timer_event()
{
    bool addNewItem=false;
    bool removeOldItem=false;
    if(!comOpenState)
    {
        serialinfo=new QSerialPortInfo();
        QStringList list1,list2;
        QMap<QString,QSerialPortInfo> tempSerialList;
        foreach(const QSerialPortInfo &info,serialinfo->availablePorts())//添加新的串口
        {
            tempSerialList.insert(info.portName(),info);
            if(!serialInfoList.contains(info.portName()))
            {
                addNewItem=true;
                serialInfoList.insert(info.portName(),info);
            }
        }

        foreach(const QSerialPortInfo &info,serialInfoList)//删除已移除的串口
        {
            if(!tempSerialList.contains(info.portName()))
            {
                removeOldItem=true;
                serialInfoList.remove(info.portName());
            }
        }
        if(addNewItem||removeOldItem)
        {
            QMap<QString, QSerialPortInfo>::const_iterator i;
            for( i=serialInfoList.begin(); i!=serialInfoList.end(); ++i)
            {
                if(i.key().length()>4)
                {
                    list2.append(i.key());
                }else{
                    list1.append(i.key());
                }
            }
            list1.sort();
            list2.sort();
            list1<<list2;
            ui->comNumOpt->clear();
            ui->comNumOpt->addItems(list1);
            qDebug()<<list1;
        }
    }
}

void UpperMain::timer_send(bool state)
{
    if(!comOpenState) {ui->txTimerCkb->setChecked(false);return;}
    int timerCount=ui->txTimerCountSpb->value();
    if(state)
    {
        bool event_confirm=QMessageBox::question(this,"定时任务提醒","即将开启定时重复发送任务！发送框和发送按钮将被锁定!\t\n是否确定开启？\t","取消","确定");
        if(!event_confirm)
        {
            ui->txTimerCkb->setChecked(false);
            return;
        }
        ui->txSendBtn->setText("执行中");
        ui->txSendBtn->setDisabled(state);
        ui->txText->setDisabled(state);
        send_event_timer=new QTimer(this);
        connect(send_event_timer, SIGNAL(timeout()), this, SLOT(tx_send_clicked()));
        send_event_timer->start(timerCount);
    }else{
        ui->txSendBtn->setText("发送");
        ui->txSendBtn->setDisabled(state);
        ui->txText->setDisabled(state);
        send_event_timer->stop();
    }
}


void UpperMain::cmd_updated(int cmdId,QString cmdName,QString cmdContent)
{
    cmd[cmdId]=cmdContent;
    cmdbtn[cmdId]->setText(cmdName);
    ui->txText->setPlainText(cmd[cmdId]);
}

void UpperMain::initPara()
{
    cmdbtn[0]=ui->cmd1;
    cmdbtn[1]=ui->cmd2;
    cmdbtn[2]=ui->cmd3;
    cmdbtn[3]=ui->cmd4;
    cmdbtn[4]=ui->cmd5;
    cmdbtn[5]=ui->cmd6;

    cmd[0]="cmd1";
    cmd[1]="cmd2";
    cmd[2]="cmd3";
    cmd[3]="cmd4";
    cmd[4]="cmd5";
    cmd[4]="cmd6";


    comOpenState=false;
    rxPauseState=true;
    rxBitsCounter=0;
    txBitsCounter=0;

    serialinfo=new QSerialPortInfo();
    QStringList list1,list2;
    foreach(const QSerialPortInfo &info,serialinfo->availablePorts())
    {
        serialInfoList.insert(info.portName(),info);
        if(info.portName().length()>4)//按顺序排列COM
        {
            list2.append(info.portName());
        }else{
            list1.append(info.portName());
        }
    }
    list1.sort();
    list2.sort();
    list1<<list2;
    ui->comNumOpt->addItems(list1);
    qDebug()<<list1;
}


void UpperMain::com_open_clicked()
{
    if(!comOpenState)
    {
        QString comPort=ui->comNumOpt->currentText();
        int baudRate=ui->baudRateOpt->currentText().toInt();
        QString baudRateStr=ui->baudRateOpt->currentText();
        int dataBit=ui->dataBitOpt->currentText().toInt();
        QString dataBitStr=ui->dataBitOpt->currentText();
        int parityBit=ui->parityBitOpt->currentIndex();
        QString parityBitStr=ui->parityBitOpt->currentText();
        int stopBit=ui->stopBitOpt->currentText().toInt();
        QString stopBitStr=ui->stopBitOpt->currentText();
        serial->setPortName(comPort);
        serial->setBaudRate(baudRate);
        switch (dataBit) {
        case 5:
            serial->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serial->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serial->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serial->setDataBits(QSerialPort::Data8);
            break;
        }
        switch (parityBit) {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serial->setParity(QSerialPort::OddParity);
            break;
        case 2:
            serial->setParity(QSerialPort::EvenParity);
            break;
        case 3:
            serial->setParity(QSerialPort::MarkParity);
            break;
        }
        switch (stopBit) {
        case 1:
            serial->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serial->setStopBits(QSerialPort::TwoStop);
            break;
        default://1.5个停止位
            serial->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        }

        bool state=serial->open(QIODevice::ReadWrite);
        if(state)
        {
            comOpenState=true;
            rxPauseState=false;
            ui->openBtn->setText("断开");
            ui->comState->setText("开启 "+comPort+" "+baudRateStr+" "+dataBitStr+" "+parityBitStr+" "+stopBitStr);
            currentSerial=serialInfoList[comPort];
            //监听串口输入事件
            connect(serial,&QSerialPort::readyRead,this,&UpperMain::rx_read_listen);
            //设定时器定时查询串口
//            rx_listen_timer = new QTimer(this);
//            connect(rx_listen_timer, SIGNAL(timeout()), this, SLOT(rx_read_listen()));
//            rx_listen_timer->start(RX_EVENT_TIMEOUT);//每100ms读一次
            UpperMain::setWindowTitle(APP_NAME+"-"+comPort);
        }
        qDebug()<<state<<baudRate<<dataBit<<parityBit<<stopBit<<currentSerial.description();

        if(!state)
        {
            QMessageBox::information(this,"异常提示","串口打开失败！请检查串口是否被占用！","确定");
        }

    }else{
        if(serial->isOpen()){
            UpperMain::setWindowTitle(APP_NAME);
            serial->close();
            comOpenState=false;
            rxPauseState=true;
            ui->openBtn->setText("连接");
            ui->comState->setText("关闭");
        }
    }
    ui->comNumOpt->setDisabled(comOpenState);
    ui->baudRateOpt->setDisabled(comOpenState);
    ui->dataBitOpt->setDisabled(comOpenState);
    ui->parityBitOpt->setDisabled(comOpenState);
    ui->stopBitOpt->setDisabled(comOpenState);
}

void UpperMain::rx_read_listen()
{
    if(!comOpenState){return;}

    if(!currentSerial.isValid())//串口掉线
    {
        serial->close();
        comOpenState=false;
        rxPauseState=true;
        ui->comNumOpt->setDisabled(comOpenState);
        ui->baudRateOpt->setDisabled(comOpenState);
        ui->dataBitOpt->setDisabled(comOpenState);
        ui->parityBitOpt->setDisabled(comOpenState);
        ui->stopBitOpt->setDisabled(comOpenState);

        ui->txText->setEnabled(true);//取消定时任务
        ui->txSendBtn->setEnabled(true);
        ui->txSendBtn->setText("发送");
        ui->txTimerCkb->setChecked(false);
        send_event_timer->stop();//取消定时器

        ui->openBtn->setText("连接");
        ui->comState->setText("关闭");
        QMessageBox::warning(this, tr("串口异常"), "串口连接中断，请检查是否正确连接！");
        return;
    }

    int showTime=ui->rxShowTimeCkb->checkState();
    int isHex=ui->rxTypeOpt->currentIndex();
    int isReadNewLine=ui->rxAutoNewlineCkb->checkState();
    QByteArray readComData;

    if(isReadNewLine){
        readComData = serial->readLine();//按行读取
    }else{
        readComData = serial->readAll();//读取串口数据
    }

    if(rxPauseState){readComData.clear();return;};

    if(readComData.length()>0)
    {
        if(showTime)//加上时间戳
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("[yyyy-MM-dd hh:mm:ss.zzz]");
            ui->rxText->append(current_date+" >>>");
        }

        if(isHex)//HEX显示
        {
            QString hexData=readComData.toHex().toUpper();
            QString hexStr="";
            for(int i=0;i<hexData.length();i++)
            {
                if(i%2==0)
                    hexStr+=" ";
                hexStr+=hexData.at(i);
            }
            ui->rxText->append(hexStr.trimmed());
        }else{
            ui->rxText->append(readComData);
        }
        qDebug()<<readComData;
        rxBitsCounter+=readComData.size();
        ui->rxBitCountNum->setNum(rxBitsCounter);
    }
    readComData.clear();
}

void UpperMain::tx_send_clicked()
{

    int showTime=ui->rxShowTimeCkb->checkState();
    int isEcho=ui->rxEchoTxCkb->checkState();
    int isHex=ui->txTypeOpt->currentIndex();
    int newLineType=ui->txNewlineTypeOpt->currentIndex();

    QString txText=ui->txText->toPlainText();
    QString temp;
    QByteArray sendData;

    if(!comOpenState||txText.length()<=0){return;}

    /*if(ItemModel->findItems(txText).length()==0){//查找历史记录 保证不重复
        QStandardItem *item = new QStandardItem(txText);
        ItemModel->appendRow(item);
        ui->txHistoryList->setModel(ItemModel);
    }*/

    if(isEcho&&!rxPauseState)
    {
        if(showTime)//加上时间戳
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("[yyyy-MM-dd hh:mm:ss.zzz]");
            ui->rxText->append(current_date+" <<<");
        }
        //        ui->rxText->append("<strong color=\"green\">"+txText+"</strong>");
        ui->rxText->append("<font color=\"green\">"+txText+"</font>");
    }

    if(isHex)//发送HEX
    {
        switch (newLineType) {//加上换行符 不显示换行符
        case 1:
            txText+="A0D0";
            break;
        case 2:
            txText+="D0";
            break;
        case 3:
            txText+="A0";
            break;
        }
        qDebug()<<convertStringToHex(txText);
        sendData=convertStringToHex(txText);
    }else{
        switch (newLineType) {//加上换行符
        case 1:
            txText+="\r\n";
            break;
        case 2:
            txText+="\r";
            break;
        case 3:
            txText+="\n";
            break;
        }
        sendData = txText.toLocal8Bit();//解决发送 中文乱码
    }
    qDebug()<<sendData;
    serial->write(sendData);

    txBitsCounter+=txText.length();
    ui->txBitCountNum->setNum(txBitsCounter);
}

void UpperMain::initConf()
{
    QSettings *config = new QSettings("config.ini", QSettings::IniFormat);
    config->setIniCodec("UTF-8");
    for(int i=0;i<6;i++)
    {
        QString t_CmdName=config->value("cmd/cmd"+QString::number(i+1)+"Name").toString();
        QString t_CmdContent=config->value("cmd/cmd"+QString::number(i+1)+"Content").toString();
        cmdbtn[i]->setText(t_CmdName.length()>0?t_CmdName:"CMD"+QString::number(i+1));
        cmd[i]=t_CmdContent.length()>0?t_CmdContent:"cmd"+QString::number(i+1);
    }

    ui->baudRateOpt->setCurrentIndex(config->value("connect/baudRate").toInt());
    ui->dataBitOpt->setCurrentIndex(config->value("connect/dataBit").toInt());
    ui->parityBitOpt->setCurrentIndex(config->value("connect/parityBit").toInt());
    ui->stopBitOpt->setCurrentIndex(config->value("connect/stopBit").toInt());

    ui->rxTypeOpt->setCurrentIndex(config->value("rx/rxType").toInt());
    ui->rxAutoNewlineCkb->setChecked(config->value("rx/autoNewline").toBool());
    ui->rxShowTimeCkb->setChecked(config->value("rx/showTime").toBool());
    ui->rxEchoTxCkb->setChecked(config->value("rx/txEcho").toBool());

    ui->txTypeOpt->setCurrentIndex(config->value("tx/txType").toInt());
    ui->txTimerCountSpb->setValue(config->value("tx/timerCount").toInt());
    ui->txNewlineTypeOpt->setCurrentIndex(config->value("tx/newLineType").toInt());
}

void UpperMain::saveConf()
{
    QSettings *conf = new QSettings("config.ini", QSettings::IniFormat);
    conf->beginGroup("connect");
    conf->setValue("baudRate",ui->baudRateOpt->currentIndex());
    conf->setValue("dataBit",ui->dataBitOpt->currentIndex());
    conf->setValue("parityBit",ui->parityBitOpt->currentIndex());
    conf->setValue("stopBit",ui->stopBitOpt->currentIndex());
    conf->endGroup();

    conf->beginGroup("rx");
    conf->setValue("rxType",ui->rxTypeOpt->currentIndex());
    conf->setValue("autoNewline",ui->rxAutoNewlineCkb->checkState());
    conf->setValue("showTime",ui->rxShowTimeCkb->checkState());
    conf->setValue("txEcho",ui->rxEchoTxCkb->checkState());
    conf->endGroup();

    conf->beginGroup("tx");
    conf->setValue("txType",ui->txTypeOpt->currentIndex());
    conf->setValue("timer",ui->txTimerCkb->checkState());
    conf->setValue("timerCount",ui->txTimerCountSpb->value());
    conf->setValue("newlineType",ui->txNewlineTypeOpt->currentIndex());
    conf->endGroup();

    conf->beginGroup("cmd");
    conf->setValue("cmd1Name",ui->cmd1->text());
    conf->setValue("cmd1Content",cmd[0]);
    conf->setValue("cmd2Name",ui->cmd2->text());
    conf->setValue("cmd2Content",cmd[1]);
    conf->setValue("cmd3Name",ui->cmd3->text());
    conf->setValue("cmd3Content",cmd[2]);
    conf->setValue("cmd4Name",ui->cmd4->text());
    conf->setValue("cmd4Content",cmd[3]);
    conf->setValue("cmd5Name",ui->cmd5->text());
    conf->setValue("cmd5Content",cmd[4]);
    conf->setValue("cmd6Name",ui->cmd6->text());
    conf->setValue("cmd6Content",cmd[5]);
    conf->endGroup();

}

QByteArray UpperMain::convertStringToHex(QString &hexStr)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = hexStr.length();
    senddata.resize(len/2);
    char lstr, hstr;
    for(int i = 0; i < len; )
    {
        //将第一个不为' '的字符赋给hstr;
        hstr = hexStr[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        //当i >= len时，跳出循环
        if(i >= len)
            break;
        //当i < len时，将下一个字符赋值给lstr;
        lstr = hexStr[i].toLatin1();
        //将hstr和lstr转换为0-15的对应数值
        hexdata = convertCharToHex(hstr);
        lowhexdata = convertCharToHex(lstr);
        //
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

char UpperMain::convertCharToHex(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}


bool UpperMain::includeChinese(QString str)
{
    for(int i=0; i<str.length(); i++)
    {
        QChar h = str.at(i);
        if(  (h<0XA0B0 && h>122) || h>0xfef7 || (h>90 && h<97) ||(h>57 && h<65) || h<48)
            return false;
    }
    return true;
}
