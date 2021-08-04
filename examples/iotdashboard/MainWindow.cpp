/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"
#include "MainContent.h"
#include "MenuBar.h"

#include <QskLinearBox.h>

MainWindow::MainWindow()
{
    setPreferredSize( QSize( 1024, 600 ) );
    setTitle( "IOT dashboard" );

    auto layout = new QskLinearBox( Qt::Horizontal, contentItem() );
    layout->setSpacing( 0 );

    (void) new MenuBar( layout );
    (void) new MainContent( layout );
}

#include "moc_MainWindow.cpp"
