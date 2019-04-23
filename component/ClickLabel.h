#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    MyLabel(QWidget *parent=nullptr);

protected:
    virtual void mousePressEvent(QMouseEvent *ev);

signals:
    void labelClicked();

};

#endif // CLICKLABEL_H
