/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskWindow.h>

class QskSkin;

class MainWindow : public QskWindow
{
    Q_OBJECT

  public:
    MainWindow();
    void setGraphic( const QString& );

  private Q_SLOTS:
    void setGraphicRoles( QskSkin* );
};
