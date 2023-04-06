/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
