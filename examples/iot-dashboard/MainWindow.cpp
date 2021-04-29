/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"

#include "MainContent.h"
#include "MenuBar.h"

#include <QskLinearBox.h>

MainWindow::MainWindow()
    : QskWindow()
    , m_mainLayout( new QskLinearBox( Qt::Horizontal, contentItem() ) )
    , m_menuBar( new MenuBar( m_mainLayout ) )
    , m_mainContent( new MainContent( m_mainLayout ) )
{
    setPreferredSize( { 1024, 600 } );
    setTitle( "IOT dashboard" );

    m_mainLayout->setSpacing( 0 );
}

#include "moc_MainWindow.cpp"
