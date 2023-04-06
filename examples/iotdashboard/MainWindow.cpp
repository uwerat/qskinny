/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
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
