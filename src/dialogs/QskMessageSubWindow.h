/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MESSAGE_SUB_WINDOW_H
#define QSK_MESSAGE_SUB_WINDOW_H 1

#include "QskInputSubWindow.h"

class QSK_EXPORT QskMessageSubWindow : public QskInputSubWindow
{
    Q_OBJECT

    using Inherited = QskInputSubWindow;

  public:
    QskMessageSubWindow( QQuickItem* parent = nullptr );
    ~QskMessageSubWindow() override;
};

#endif
