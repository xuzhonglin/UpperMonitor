#include "uppermain.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UpperMain w;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    w.setWindowTitle(APP_NAME);
    w.setWindowFlags(w.windowFlags() &~ Qt::WindowMaximizeButtonHint);
    w.setFixedSize(w.width(),w.height());
    w.setWindowState(Qt::WindowState::WindowActive);
    w.show();
    return a.exec();
}
