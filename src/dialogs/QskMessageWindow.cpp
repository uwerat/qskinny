/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMessageWindow.h"
#include "QskMessageSubWindow.h"

QskMessageWindow::QskMessageWindow( QWindow* parent ):
    Inherited( parent )
{
    setSubWindow( new QskMessageSubWindow() );
}

QskMessageWindow::~QskMessageWindow()
{
}

#include "moc_QskMessageWindow.cpp"
