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

void GridAccessor::setRowSizeHint( int row, Qt::SizeHint which, int height )
{
    setSizeHint( row, Qt::Vertical, which, height );
}

void GridAccessor::setColumnSizeHint( int column, Qt::SizeHint which, int width )
{
    setSizeHint( column, Qt::Horizontal, which, width );
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

void GridAccessor::setRowStretchFactor( int row, int stretch )
{
    setStretchFactor( row, Qt::Vertical, stretch );
}

void GridAccessor::setColumnStretchFactor( int column, int stretch )
{
    setStretchFactor( column, Qt::Vertical, stretch );
}

void GridAccessor::setMinimumWidthAt( int index, int hint )
{
    setSizeHintAt( index, Qt::Horizontal, Qt::MinimumSize, hint );
}

void GridAccessor::setMinimumHeightAt( int index, int hint )
{
    setSizeHintAt( index, Qt::Vertical, Qt::MinimumSize, hint );
}

void GridAccessor::setMinimumSizeAt( int index, const QSize& size )
{
    setMinimumWidthAt( index, size.width() );
    setMinimumHeightAt( index, size.height() );
}

void GridAccessor::setPreferredWidthAt( int index, int hint )
{
    setSizeHintAt( index, Qt::Horizontal, Qt::PreferredSize, hint );
}

void GridAccessor::setPreferredHeightAt( int index, int hint )
{
    setSizeHintAt( index, Qt::Vertical, Qt::PreferredSize, hint );
}

void GridAccessor::setPreferredSizeAt( int index, const QSize& size )
{
    setPreferredWidthAt( index, size.width() );
    setPreferredHeightAt( index, size.height() );
}

void GridAccessor::setMaximumWidthAt( int index, int hint )
{
    setSizeHintAt( index, Qt::Horizontal, Qt::MaximumSize, hint );
}

void GridAccessor::setMaximumHeightAt( int index, int hint )
{
    setSizeHintAt( index, Qt::Vertical, Qt::MaximumSize, hint );
}

void GridAccessor::setMaximumSizeAt( int index, const QSize& size )
{
    setMaximumWidthAt( index, size.width() );
    setMaximumHeightAt( index, size.height() );
}

void GridAccessor::setFixedWidthAt( int index, int hint )
{
    setMinimumWidthAt( index, hint );
    setMaximumWidthAt( index, hint );
}

void GridAccessor::setFixedHeightAt( int index, int hint )
{
    setMinimumHeightAt( index, hint );
    setMaximumHeightAt( index, hint );
}

void GridAccessor::setFixedSizeAt( int index, const QSize& size )
{
    setMinimumSizeAt( index, size );
    setMaximumSizeAt( index, size );
}

void GridAccessor::setSizePoliciesAt(
    int index, int horizontalPolicy, int verticalPolicy )
{
    setSizePolicyAt( index, Qt::Horizontal, horizontalPolicy );
    setSizePolicyAt( index, Qt::Vertical, verticalPolicy );
}
