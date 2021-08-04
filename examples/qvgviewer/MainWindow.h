/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskWindow.h>

class QskTabView;
class QskSkin;

class MainWindow : public QskWindow
{
    Q_OBJECT

  public:
    MainWindow();

  private Q_SLOTS:
    void setDarknessMode( bool on );
    void setGraphicRoles( QskSkin* );

  private:
    QskTabView* m_tabView;
};
