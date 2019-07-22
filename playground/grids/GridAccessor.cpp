/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridAccessor.h"
#include <QSize>

void GridAccessor::setSpacing( int spacing )
{
    setSpacing( Qt::Vertical | Qt::Horizontal, spacing );
}

void GridAccessor::setRowFixedHeight( int row, qreal height )
{
    setRowSizeHint( row, Qt::MinimumSize, height );
    setRowSizeHint( row, Qt::MaximumSize, height );
}

void GridAccessor::setColumnFixedWidth( int column, qreal width )
{
    setColumnSizeHint( column, Qt::MinimumSize, width );
    setColumnSizeHint( column, Qt::MaximumSize, width );
}

void GridAccessor::setMinimumWidth( int index, int hint )
{
    setSizeHint( index, Qt::Horizontal, Qt::MinimumSize, hint );
}

void GridAccessor::setMinimumHeight( int index, int hint )
{
    setSizeHint( index, Qt::Vertical, Qt::MinimumSize, hint );
}

void GridAccessor::setMinimumSize( int index, const QSize& size )
{
    setMinimumWidth( index, size.width() );
    setMinimumHeight( index, size.height() );
}

void GridAccessor::setPreferredWidth( int index, int hint )
{
    setSizeHint( index, Qt::Horizontal, Qt::PreferredSize, hint );
}

void GridAccessor::setPreferredHeight( int index, int hint )
{
    setSizeHint( index, Qt::Vertical, Qt::PreferredSize, hint );
}

void GridAccessor::setPreferredSize( int index, const QSize& size )
{
    setPreferredWidth( index, size.width() );
    setPreferredHeight( index, size.height() );
}

void GridAccessor::setMaximumWidth( int index, int hint )
{
    setSizeHint( index, Qt::Horizontal, Qt::MaximumSize, hint );
}

void GridAccessor::setMaximumHeight( int index, int hint )
{
    setSizeHint( index, Qt::Vertical, Qt::MaximumSize, hint );
}

void GridAccessor::setMaximumSize( int index, const QSize& size )
{
    setMaximumWidth( index, size.width() );
    setMaximumHeight( index, size.height() );
}

void GridAccessor::setSizePolicy(
    int index, int horizontalPolicy, int verticalPolicy )
{
    setSizePolicy( index, Qt::Horizontal, horizontalPolicy );
    setSizePolicy( index, Qt::Vertical, verticalPolicy );
}
