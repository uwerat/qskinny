/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestBox.h"
#include "GridGraphics.h"
#include "GridSkinny.h"
#include "GridWidgets.h"
#include "GridQuick.h"

TestBox::TestBox( QWidget* parent )
    : QWidget( parent )
{
    setPalette( QColor( "Lavender" ) );
    setAutoFillBackground( true );

    setContentsMargins( QMargins( 10, 10, 10, 10 ) );

    grids[Skinny] = new GridSkinny( this );
    grids[Widgets] = new GridWidgets( this );
    grids[Graphics] = new GridGraphics( this );
    grids[Quick] = new GridQuick( this );
}

TestBox::~TestBox()
{
}

void TestBox::setColumns( int columnCount )
{
    m_columnCount = qBound( 1, columnCount, static_cast< int >( GridCount ) );

    if ( testAttribute( Qt::WA_WState_Polished ) )
        layoutGrids();
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

void TestBox::setSizeHint(
    int pos, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setSizeHint( pos, orientation, which, hint );
    }
}

void TestBox::setStretchFactor(
    int pos, Qt::Orientation orientation, int stretch )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setStretchFactor( pos, orientation, stretch );
    }
}

void TestBox::setSizeHintAt(
    int index, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setSizeHintAt( index, orientation, which, hint );
    }
}

void TestBox::setSizePolicyAt(
    int index, Qt::Orientation orientation, int policy )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setSizePolicyAt( index, orientation, policy );
    }
}

void TestBox::setAlignmentAt( int index, Qt::Alignment alignment )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setAlignmentAt( index, alignment );
    }
}

void TestBox::setRetainSizeWhenHiddenAt( int index, bool on )
{
    for ( auto grid : grids )
    {
        auto accessor = dynamic_cast< GridAccessor* >( grid );
        accessor->setRetainSizeWhenHiddenAt( index, on );
    }
}

void TestBox::resizeEvent( QResizeEvent* )
{
    layoutGrids();
}

void TestBox::layoutGrids()
{
    /*
        Not using layouts here to avoid confusion
        while debugging.
     */

    const auto r = contentsRect();

    int rowCount = GridCount / m_columnCount;
    if ( rowCount * m_columnCount < GridCount )
        rowCount++;

    const int spacing = 5;

    const int width = ( r.width() - ( m_columnCount - 1 ) * spacing ) / m_columnCount;
    const int height = ( r.height() - ( rowCount - 1 ) * spacing ) / rowCount;

    int row = 0;
    int col = 0;

    for ( int i = 0; i < GridCount; i++ )
    {
        const int x = r.left() + col * ( spacing + width );
        const int y = r.top() + row * ( spacing + height );

        grids[i]->setGeometry( x, y, width, height );

        if ( ++col >= m_columnCount )
        {
            col = 0;
            row++;
        }
    }
}
