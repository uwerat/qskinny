#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QskPushButton.h>
#include <QskWindow.h>

class QskGraphicLabel;
class QskLinearBox;

class MainWindow : public QskWindow
{
public:
    MainWindow();

private:
    void addHeaderBar();
    void addMainContent();
    void addBottomBar();

    QskGraphicLabel* m_backgroundImage;
    QskLinearBox* m_layout;
};

#endif // MAINWINDOW_H
