#ifndef DOUBLECLICKBUTTON_H
#define DOUBLECLICKBUTTON_H


#include <QPushButton>

class QTimer;

class DoubleClickedButton : public QPushButton
{
    Q_OBJECT

public:
    DoubleClickedButton(QWidget *parent=Q_NULLPTR);
    DoubleClickedButton(const QString &text, QWidget *parent = Q_NULLPTR);
    DoubleClickedButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);

    ~DoubleClickedButton();

protected:
    virtual bool event(QEvent* event);

signals:
    void singleClicked();
    void doubleClicked();

private:
    bool    m_bDoubleClicked;
    QTimer* m_single_clicked_timer;
};

#endif // DOUBLECLICKBUTTON_H
