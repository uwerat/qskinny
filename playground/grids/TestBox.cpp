/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestBox.h"
#include "GridGraphics.h"
#include "GridSkinny.h"
#include "GridWidgets.h"

TestBox::TestBox( QWidget* parent )
    : QWidget( parent )
{
    setPalette( QColor( "Lavender" ) );
    setAutoFillBackground( true );

    setContentsMargins( QMargins( 10, 10, 10, 10 ) );

    grids[Skinny] = new GridSkinny( this );
    grids[Widgets] = new GridWidgets( this );
    grids[Graphics] = new GridGraphics( this );
}

TestBox::~TestBox()
{
}

void TestBox::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->insert( colorName,
            row, column, rowSpan, columnSpan );
    }
}

void TestBox::setSpacing( Qt::Orientations orientations, int spacing )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setSpacing( orientations, spacing );
    }
}

void TestBox::setRowSizeHint( int row, Qt::SizeHint which, int height )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setRowSizeHint( row, which, height );
    }
}

void TestBox::setColumnSizeHint( int column, Qt::SizeHint which, int width )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setColumnSizeHint( column, which, width );
    }
}

void TestBox::setSizeHint(
    int index, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setSizeHint( index, orientation, which, hint );
    }
}

void TestBox::setSizePolicy(
    int index, Qt::Orientation orientation, int policy )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setSizePolicy( index, orientation, policy );
    }
}

void TestBox::setAlignment( int index, Qt::Alignment alignment )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setAlignment( index, alignment );
    }
}

void TestBox::setRetainSizeWhenHidden( int index, bool on )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setRetainSizeWhenHidden( index, on );
    }
}

void TestBox::resizeEvent( QResizeEvent* )
{
    /*
        Not using layouts here to avoid confusion
        while debugging.
     */

    const auto r = contentsRect();
    const auto sz = 0.5 * r.size() - QSize( 5, 5 );

    grids[ 0 ]->move( r.left(), r.top() );
    grids[ 1 ]->move( r.right() - sz.width(), r.top() );
    grids[ 2 ]->move( r.left(), r.bottom() - sz.height() );

    for ( auto grid : grids )
        grid->resize( sz );
}
