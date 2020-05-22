#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QskWindow.h>

class QskLinearBox;

class MainWindow : public QskWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    void addMenuBar();
    void addMainContent();

    QskLinearBox* m_mainLayout;
};

#endif // MAINWINDOW_H
