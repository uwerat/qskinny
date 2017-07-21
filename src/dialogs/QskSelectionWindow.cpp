/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSelectionWindow.h"
#include "QskSelectionSubWindow.h"

QskSelectionWindow::QskSelectionWindow( QWindow* parent ):
    Inherited( parent )
{
    auto subWindow = new QskSelectionSubWindow();

    connect( subWindow, &QskSelectionSubWindow::selectedRowChanged,
        this, &QskSelectionWindow::selectedRowChanged );

    connect( subWindow, &QskSelectionSubWindow::selectedEntryChanged,
        this, &QskSelectionWindow::selectedEntryChanged );

    connect( subWindow, &QskSelectionSubWindow::entriesChanged,
        this, &QskSelectionWindow::entriesChanged );

    setSubWindow( subWindow );
}

QskSelectionWindow::~QskSelectionWindow()
{
}

void QskSelectionWindow::setEntries( const QStringList& entries )
{
    auto subWindow = static_cast< QskSelectionSubWindow* >( this->subWindow() );
    subWindow->setEntries( entries );
}

QStringList QskSelectionWindow::entries() const
{
    auto w = static_cast< const QskSelectionSubWindow* >( this->subWindow() );
    return w->entries();
}

void QskSelectionWindow::setSelectedRow( int row )
{
    auto subWindow = static_cast< QskSelectionSubWindow* >( this->subWindow() );
    subWindow->setSelectedRow( row );
}

int QskSelectionWindow::selectedRow() const
{
    auto subWindow = static_cast< const QskSelectionSubWindow* >( this->subWindow() );
    return subWindow->selectedRow();
}

QString QskSelectionWindow::selectedEntry() const
{
    auto subWindow = static_cast< const QskSelectionSubWindow* >( this->subWindow() );
    return subWindow->selectedEntry();
}

#include "moc_QskSelectionWindow.cpp"
