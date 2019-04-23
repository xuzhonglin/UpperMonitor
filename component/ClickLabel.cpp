#include "ClickLabel.h"

MyLabel::MyLabel(QWidget *parent):QLabel(parent)
{
}

void MyLabel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    emit labelClicked();
}
