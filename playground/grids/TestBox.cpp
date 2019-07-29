/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestBox.h"
#include "GridGraphics.h"
#include "GridSkinny.h"
#include "GridWidgets.h"
#include "GridQuick.h"
#include <QLabel>

TestBox::TestBox( QWidget* parent )
    : QWidget( parent )
    , m_grids {}
{
    setPalette( QColor( "Lavender" ) );
    setAutoFillBackground( true );

    setContentsMargins( QMargins( 10, 10, 10, 10 ) );

    for ( int i = 0; i < GridCount; i++ )
        m_labels[i] = new QLabel( this );
}

TestBox::~TestBox()
{
}

void TestBox::reset()
{
    setColumns( 2 );

    for ( auto grid : m_grids )
        delete grid;

    m_grids[Skinny] = new GridSkinny( this );
    m_grids[Widgets] = new GridWidgets( this );
    m_grids[Graphics] = new GridGraphics( this );
    m_grids[Quick] = new GridQuick( this );

    for ( int i = 0; i < GridCount; i++ )
    {
        m_grids[i]->show();

        m_labels[i]->show();
        m_labels[i]->raise();
        m_labels[i]->setText( QString() );
    }

    updateGeometry();
}

void TestBox::enableGrid( int index, bool on )
{
    m_grids[index]->setVisible( on );
    m_labels[index]->setVisible( on );
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
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->insert( colorName,
                row, column, rowSpan, columnSpan );
        }
    }
}

void TestBox::setSpacing( Qt::Orientations orientations, int spacing )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setSpacing( orientations, spacing );
        }
    }
}

void TestBox::setSizeHint(
    int pos, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setSizeHint( pos, orientation, which, hint );
        }
    }
}

void TestBox::setStretchFactor(
    int pos, Qt::Orientation orientation, int stretch )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setStretchFactor( pos, orientation, stretch );
        }
    }
}

void TestBox::setSizeHintAt(
    int index, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setSizeHintAt( index, orientation, which, hint );
        }
    }
}

void TestBox::setSizePolicyAt(
    int index, Qt::Orientation orientation, int policy )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setSizePolicyAt( index, orientation, policy );
        }
    }
}

void TestBox::setAlignmentAt( int index, Qt::Alignment alignment )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setAlignmentAt( index, alignment );
        }
    }
}

void TestBox::setRetainSizeWhenHiddenAt( int index, bool on )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setRetainSizeWhenHiddenAt( index, on );
        }
    }
}

void TestBox::setVisibleAt( int index, bool on )
{
    for ( auto grid : m_grids )
    {
        if ( grid->isVisibleTo( this ) )
        {
            auto accessor = dynamic_cast< GridAccessor* >( grid );
            accessor->setVisibleAt( index, on );
        }
    }
}

QSize TestBox::preferredSize() const
{
    return QSize( -1, -1 );
}

bool TestBox::event( QEvent* event )
{
    if ( event->type() == QEvent::LayoutRequest )
        layoutGrids();

    return QWidget::event( event );
}

void TestBox::resizeEvent( QResizeEvent* )
{
    layoutGrids();
}

void TestBox::updateLabels()
{
    const QString texts[ GridCount ] = { "Skinny", "Widgets", "Graphics", "Quick" };

    for ( int i = 0; i < GridCount; i++ )
    {
        auto accessor = dynamic_cast< GridAccessor* >( m_grids[i] );
        auto label = m_labels[i];

        const auto hint = accessor->preferredSize();

        QString text = texts[i];
        text += QString( ": ( %1x%2 )" ).arg( hint.width() ).arg( hint.height() );

        label->setText( text );
    }
}

void TestBox::layoutGrids()
{
    /*
        Not using layouts here to avoid confusion
        while debugging.
     */

    const auto r = contentsRect();

    int gridCount = 0;
    for ( int i = 0; i < GridCount; i++ )
    {
        if ( m_grids[i]->isVisibleTo( this ) )
            gridCount++;
    }

    int columnCount = qMin( gridCount, m_columnCount );
    int rowCount = gridCount / columnCount;
    if ( rowCount * columnCount < gridCount )
        rowCount++;

    const int spacing = 5;

    const int width = ( r.width() - ( columnCount - 1 ) * spacing ) / columnCount;
    const int height = ( r.height() - ( rowCount - 1 ) * spacing ) / rowCount;

    int row = 0;
    int col = 0;

    for ( int i = 0; i < GridCount; i++ )
    {
        if ( !m_grids[i]->isVisibleTo( this ) )
            continue;

        const int x = r.left() + col * ( spacing + width );
        const int y = r.top() + row * ( spacing + height );

        const QRect rect( x, y, width, height );
        m_grids[i]->setGeometry( rect );

        const auto sz = m_labels[i]->sizeHint();

        m_labels[i]->setGeometry(
            rect.right() - sz.width() - 10, rect.top() + 10,
            sz.width(), sz.height() );

        if ( ++col >= columnCount )
        {
            col = 0;
            row++;
        }
    }
}
