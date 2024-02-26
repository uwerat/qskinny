/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskMainView.h>

class QskModelObjectBinder;

class MainView : public QskMainView
{
  public:
    MainView( QQuickItem* = nullptr );

  private:
    void toogleRow();

    QskModelObjectBinder* m_binder;
};
