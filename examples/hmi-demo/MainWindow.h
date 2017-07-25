#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QskWindow.h>

class QskLinearBox;

class MainWindow : public QskWindow
{
public:
    MainWindow();

private:
    void addHeaderBar();
    void addMainContent();
    void addBottomBar();

    QskLinearBox* m_layout;
};

#endif // MAINWINDOW_H
