/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridGraphics.h"

#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QDebug>

namespace
{
    class Rectangle : public QGraphicsWidget
    {
      public:
        Rectangle( const QByteArray& colorName )
            : m_colorName( colorName )
        {
            setObjectName( colorName );

            setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
            setPreferredSize( 50, 50 );

            setPalette( QColor( colorName.constData() ) );
            setAutoFillBackground( true );
        }

        void resizeEvent( QGraphicsSceneResizeEvent* event ) override
        {
            QGraphicsWidget::resizeEvent( event );
            //qDebug() << m_color << size();
        }

      private:
        const QByteArray m_colorName;
    };
}

GridGraphics::GridGraphics( QWidget* parent )
    : QGraphicsView( parent )
{
    setFrameStyle( QFrame::NoFrame );
    setContentsMargins( QMargins() );

    auto grid = new QGraphicsWidget();
    m_layout = new QGraphicsGridLayout( grid );

    auto scene = new QGraphicsScene();
    scene->addItem( grid );

    setScene( scene );
}

GridGraphics::~GridGraphics()
{
}

void GridGraphics::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
    m_layout->addItem( new Rectangle( colorName ),
        row, column, rowSpan, columnSpan );
}

void GridGraphics::setSpacing( Qt::Orientations orientations, int spacing )
{
    if ( orientations & Qt::Horizontal )
        m_layout->setHorizontalSpacing( spacing );

    if ( orientations & Qt::Vertical )
        m_layout->setVerticalSpacing( spacing );
}

void GridGraphics::setRowSizeHint( int row, Qt::SizeHint which, int height )
{
    switch( static_cast< int >( which ) )
    {
        case Qt::MinimumSize:
        {
            m_layout->setRowMinimumHeight( row, height );
            break;
        }
        case Qt::PreferredSize:
        {
            m_layout->setRowPreferredHeight( row, height );
            break;
        }
        case Qt::MaximumSize:
        {
            m_layout->setRowMaximumHeight( row, height );
            break;
        }
    }
}

void GridGraphics::setColumnSizeHint( int column, Qt::SizeHint which, int width )
{
    switch( static_cast< int >( which ) )
    {
        case Qt::MinimumSize:
        {
            m_layout->setColumnMinimumWidth( column, width );
            break;
        }
        case Qt::PreferredSize:
        {
            m_layout->setColumnPreferredWidth( column, width );
            break;
        }
        case Qt::MaximumSize:
        {
            m_layout->setColumnMaximumWidth( column, width );
            break;
        }
    }

}

void GridGraphics::setSizeHint(
    int index, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        switch( static_cast< int >( which ) )
        {
            case Qt::MinimumSize:
            {
                if ( orientation == Qt::Horizontal )
                    layoutItem->setMinimumWidth( hint );
                else
                    layoutItem->setMinimumHeight( hint );
                break;
            }
            case Qt::PreferredSize:
            {
                if ( orientation == Qt::Horizontal )
                    layoutItem->setPreferredWidth( hint );
                else
                    layoutItem->setPreferredHeight( hint );

                break;
            }
            case Qt::MaximumSize:
            {
                if ( orientation == Qt::Horizontal )
                    layoutItem->setMaximumWidth( hint );
                else
                    layoutItem->setMaximumHeight( hint );

                break;
            }
        }

    }
}

void GridGraphics::setSizePolicy( int index, Qt::Orientation orientation, int policy )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        const auto qPolicy = static_cast< QSizePolicy::Policy >( policy & 0xF );

        const bool isConstrained = policy & ( 1 << 4 );

        auto sizePolicy = layoutItem->sizePolicy();

        if ( orientation == Qt::Horizontal )
        {
            sizePolicy.setHorizontalPolicy( qPolicy );
            sizePolicy.setWidthForHeight( isConstrained );
        }
        else
        {
            sizePolicy.setVerticalPolicy( qPolicy );
            sizePolicy.setHeightForWidth( isConstrained );
        }

        layoutItem->setSizePolicy( sizePolicy );
    }
}

void GridGraphics::setAlignment( int index, Qt::Alignment alignment )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
        m_layout->setAlignment( layoutItem, alignment );
}

void GridGraphics::setRetainSizeWhenHidden( int index, bool on )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        auto sizePolicy = layoutItem->sizePolicy();
        sizePolicy.setRetainSizeWhenHidden( on );
        layoutItem->setSizePolicy( sizePolicy );
    }
}

void GridGraphics::resizeEvent( QResizeEvent* )
{
    auto grid = static_cast< QGraphicsWidget* >( scene()->items().last() );
    grid->resize( contentsRect().size() );
}
