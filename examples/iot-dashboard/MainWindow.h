/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QskWindow.h>

class MainContent;
class MenuBar;
class QskLinearBox;

class MainWindow : public QskWindow
{
        Q_OBJECT

    public:
        MainWindow();

    private:
        QskLinearBox* m_mainLayout;
        MenuBar* m_menuBar;
        MainContent* m_mainContent;
};

#endif // MAINWINDOW_H
