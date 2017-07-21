/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStackBoxAnimator.h"
#include "QskLayoutItem.h"
#include "QskLayoutEngine.h"
#include "QskStackBox.h"

static Qsk::Direction qskDirection(
    Qt::Orientation orientation, int from, int to, int itemCount )
{
    Qsk::Direction direction;

    if ( orientation == Qt::Horizontal  )
    {
        direction = Qsk::RightToLeft;

        if ( to > from )
        {
            const bool isWrapping = ( from == 0 ) && ( to == itemCount - 1 );

            if ( !isWrapping )
                direction = Qsk::LeftToRight;
        }
        else
        {
            const bool isWrapping = ( to == 0 ) && ( from == itemCount - 1 );

            if ( isWrapping )
                direction = Qsk::LeftToRight;
        }
    }
    else
    {
        direction = Qsk::BottomToTop;

        if ( to > from )
        {
            const bool isWrapping = ( from == 0 ) && ( to == itemCount - 1 );

            if ( !isWrapping )
                direction = Qsk::TopToBottom;
        }
        else
        {
            const bool isWrapping = ( to == 0 ) && ( from == itemCount - 1 );

            if ( isWrapping )
                direction = Qsk::TopToBottom;
        }
    }

    return direction;
}

QskStackBoxAnimator::QskStackBoxAnimator( QskStackBox* parent ):
    QObject( parent ),
    m_startIndex( -1 ),
    m_endIndex( -1 )
{
}

QskStackBoxAnimator::~QskStackBoxAnimator()
{
}

void QskStackBoxAnimator::setStartIndex( int index )
{
    m_startIndex = index;
}

void QskStackBoxAnimator::setEndIndex( int index )
{
    m_endIndex = index;
}

int QskStackBoxAnimator::startIndex() const
{
    return m_startIndex;
}

int QskStackBoxAnimator::endIndex() const
{
    return m_endIndex;
}

QskStackBox* QskStackBoxAnimator::stackBox() const
{
    return static_cast< QskStackBox* >( parent() );
}

QskLayoutItem* QskStackBoxAnimator::layoutItemAt( int index ) const
{
    return stackBox()->engine().layoutItemAt(
        ( index == 0 ) ? m_startIndex : m_endIndex );
}

QskStackBoxAnimator1::QskStackBoxAnimator1( QskStackBox* parent ):
    QskStackBoxAnimator( parent ),
    m_orientation( Qt::Horizontal )
{
}

QskStackBoxAnimator1::~QskStackBoxAnimator1()
{
}

void QskStackBoxAnimator1::setOrientation( Qt::Orientation orientation )
{
    if ( m_orientation != orientation )
    {
#if 1
        stop();
#endif
        m_orientation = orientation;
    }
}

Qt::Orientation QskStackBoxAnimator1::orientation() const
{
    return m_orientation;
}

void QskStackBoxAnimator1::setup()
{
    auto stackBox = this->stackBox();

    m_direction = qskDirection( m_orientation,
        startIndex(), endIndex(), stackBox->itemCount() );

    for ( int i = 0; i < 2; i++ )
    {
        QskLayoutItem* layoutItem = layoutItemAt( i );
        if ( layoutItem )
        {
            QQuickItem* item = layoutItem->item();
            const Qt::Orientation orientation = this->orientation();

            m_itemOffset[i] = ( orientation == Qt::Horizontal ) ? item->x() : item->y();

            if ( i == 1 )
            {
                // now move the new item outside of
                // the visisble area and then "show" it

                if ( orientation == Qt::Horizontal )
                    item->setX( stackBox->width() );
                else
                    item->setY( stackBox->height() );

                item->setVisible( true );
            }

            // we don't want the engine() to interfere, when
            // controlling the item by the animation

            layoutItem->setUpdateMode( QskLayoutItem::UpdateNone );
        }
    }

    m_hasClip = stackBox->clip();
    if ( !m_hasClip )
        stackBox->setClip( true );
}

void QskStackBoxAnimator1::advance( qreal value )
{
    auto stackBox = this->stackBox();

    for ( int i = 0; i < 2; i++ )
    {
        QskLayoutItem* layoutItem = layoutItemAt( i );
        if ( layoutItem == nullptr )
            continue;

        if ( layoutItem->isGeometryDirty() )
        {
            // the layout tried to replace the item, but we
            // want to have control over the position. But we
            // also lost resizing - that's why we have to do it here
            // manually

            stackBox->adjustItemAt( ( i == 0 ) ? startIndex() : endIndex() );

            QQuickItem* item = layoutItem->item();
            m_itemOffset[i] = ( m_orientation == Qt::Horizontal ) ? item->x() : item->y();
        }

        QQuickItem* item = layoutItem->item();

        if ( m_orientation == Qt::Horizontal )
        {
            const qreal off = stackBox->width() * ( value - i );

            if ( m_direction == Qsk::LeftToRight )
                item->setX( m_itemOffset[i] - off );
            else
                item->setX( m_itemOffset[i] + off );
        }
        else
        {
            const qreal off = stackBox->height() * ( value - i );

            if ( m_direction == Qsk::TopToBottom )
                item->setY( m_itemOffset[i] + off );
            else
                item->setY( m_itemOffset[i] - off );
        }
    }
}

void QskStackBoxAnimator1::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( QskLayoutItem* layoutItem = layoutItemAt( i ) )
        {
            layoutItem->setUpdateMode( QskLayoutItem::UpdateWhenVisible );
            layoutItem->item()->setVisible( i == 1 );
        }
    }

    if ( !m_hasClip )
        stackBox()->setClip( false );
}

QskStackBoxAnimator3::QskStackBoxAnimator3( QskStackBox* parent ):
    QskStackBoxAnimator( parent )
{
}

QskStackBoxAnimator3::~QskStackBoxAnimator3()
{
}

void QskStackBoxAnimator3::setup()
{
    QskLayoutItem* layoutItem = layoutItemAt( 1 );
    if ( layoutItem )
    {
        layoutItem->item()->setOpacity( 0.0 );
        layoutItem->item()->setVisible( true );
    }
}

void QskStackBoxAnimator3::advance( qreal value )
{
    QskLayoutItem* layoutItem1 = layoutItemAt( 0 );
    if ( layoutItem1 )
        layoutItem1->item()->setOpacity( 1.0 - value );

    QskLayoutItem* layoutItem2 = layoutItemAt( 1 );
    if ( layoutItem2 )
        layoutItem2->item()->setOpacity( value );
}

void QskStackBoxAnimator3::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( QskLayoutItem* layoutItem = layoutItemAt( i ) )
        {
            layoutItem->item()->setOpacity( 1.0 );
            layoutItem->item()->setVisible( i == 1 ); // not here !!
        }
    }
}

#include "moc_QskStackBoxAnimator.cpp"
