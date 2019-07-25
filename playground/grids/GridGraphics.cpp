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

      protected:
        void resizeEvent( QGraphicsSceneResizeEvent* event ) override
        {
            QGraphicsWidget::resizeEvent( event );
#if 0
            qDebug() << m_colorName << size();
#endif
        }

      private:
        const QByteArray m_colorName;
    };

    class Grid : public QGraphicsWidget
    {
      public:
        Grid()
        {
#if 0
            setPalette( QColor( "LightCyan" ) );
            setAutoFillBackground( true );
#endif
        }

      protected:
        QSizeF sizeHint( Qt::SizeHint, const QSizeF& ) const override
        {
            // we don't want to derive our size hints from the items
            return QSizeF( -1, -1 );
        }
    };
}

GridGraphics::GridGraphics( QWidget* parent )
    : QGraphicsView( parent )
{
    setFrameStyle( QFrame::NoFrame );

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    auto grid = new Grid();

    m_layout = new QGraphicsGridLayout( grid );
    m_layout->setSpacing( 5 );

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

void GridGraphics::setSizeHint(
    int pos, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    switch( static_cast< int >( which ) )
    {
        case Qt::MinimumSize:
        {
            if ( orientation == Qt::Vertical )
                m_layout->setRowMinimumHeight( pos, hint );
            else
                m_layout->setColumnMinimumWidth( pos, hint );

            break;
        }
        case Qt::PreferredSize:
        {
            if ( orientation == Qt::Vertical )
                m_layout->setRowPreferredHeight( pos, hint );
            else
                m_layout->setColumnPreferredWidth( pos, hint );

            break;
        }
        case Qt::MaximumSize:
        {
            if ( orientation == Qt::Vertical )
                m_layout->setRowMaximumHeight( pos, hint );
            else
                m_layout->setColumnMaximumWidth( pos, hint );

            break;
        }
    }
}

void GridGraphics::setStretchFactor(
    int pos, Qt::Orientation orientation, int stretch )
{
    if ( orientation == Qt::Vertical )
        m_layout->setRowStretchFactor( pos, stretch );
    else
        m_layout->setColumnStretchFactor( pos, stretch );
}

void GridGraphics::setSizeHintAt(
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

void GridGraphics::setSizePolicyAt( int index, Qt::Orientation orientation, int policy )
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

void GridGraphics::setAlignmentAt( int index, Qt::Alignment alignment )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
        m_layout->setAlignment( layoutItem, alignment );
}

void GridGraphics::setRetainSizeWhenHiddenAt( int index, bool on )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        auto sizePolicy = layoutItem->sizePolicy();
        sizePolicy.setRetainSizeWhenHidden( on );
        layoutItem->setSizePolicy( sizePolicy );
    }
}

void GridGraphics::setVisibleAt( int index, bool on )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        if ( auto item = layoutItem->graphicsItem() )
            item->setVisible( on );
    }
}

QSize GridGraphics::preferredSize() const
{
    return m_layout->preferredSize().toSize();
}

void GridGraphics::resizeEvent( QResizeEvent* )
{
    auto sceneRect = contentsRect();
#if 0
    sceneRect = sceneRect.adjusted( 10, 10, -10, -10 );
#endif
    scene()->setSceneRect( QRectF( QPointF(), sceneRect.size() ) );

    auto grid = static_cast< QGraphicsWidget* >( scene()->items().last() );
    grid->resize( sceneRect.size() );
}
