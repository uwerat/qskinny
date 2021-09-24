/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainItem.h"
#include "MainWindow.h"

#include <QskLinearBox.h>

MainWindow::MainWindow()
{
    setPreferredSize( QSize( 1024, 600 ) );
    setTitle( "IOT dashboard" );

    (void) new MainItem( contentItem() );
}

#include "moc_MainWindow.cpp"
