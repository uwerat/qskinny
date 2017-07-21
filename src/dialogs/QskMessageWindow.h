/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MESSAGE_WINDOW_H
#define QSK_MESSAGE_WINDOW_H 1

#include "QskGlobal.h"
#include "QskInputWindow.h"

class QskMessageDialogBox;

class QSK_EXPORT QskMessageWindow : public QskInputWindow
{
    Q_OBJECT

    using Inherited = QskInputWindow;

public:
    QskMessageWindow( QWindow* parent = nullptr );
    virtual ~QskMessageWindow();
};

#endif
