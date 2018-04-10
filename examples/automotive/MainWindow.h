#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QskWindow.h>

class QskLinearBox;

class QQuickItem;

class MainWindow : public QskWindow
{
public:
    MainWindow();

private:
    QQuickItem* headerBar() const;
    QQuickItem* mainContent() const;
    QQuickItem* footerBar() const;

    QskLinearBox* m_layout;
};

#endif
