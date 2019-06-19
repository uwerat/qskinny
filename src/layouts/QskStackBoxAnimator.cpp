/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStackBoxAnimator.h"
#include "QskStackBox.h"
#include "QskEvent.h"
#include "QskQuick.h"

static Qsk::Direction qskDirection(
    Qt::Orientation orientation, int from, int to, int itemCount )
{
    Qsk::Direction direction;

    if ( orientation == Qt::Horizontal )
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

QskStackBoxAnimator::QskStackBoxAnimator( QskStackBox* parent )
    : QObject( parent )
    , m_startIndex( -1 )
    , m_endIndex( -1 )
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

QQuickItem* QskStackBoxAnimator::itemAt( int index ) const
{
    return stackBox()->itemAtIndex(
        ( index == 0 ) ? m_startIndex : m_endIndex );
}

QskStackBoxAnimator1::QskStackBoxAnimator1( QskStackBox* parent )
    : QskStackBoxAnimator( parent )
    , m_orientation( Qt::Horizontal )
    , m_isDirty( false )
    , m_hasClip( false )
{
    // catching geometryChanges to know about resizing
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

    m_hasClip = stackBox->clip();
    if ( !m_hasClip )
        stackBox->setClip( true );

    stackBox->installEventFilter( this );
    m_isDirty = true;
}

void QskStackBoxAnimator1::advance( qreal value )
{
    auto stackBox = this->stackBox();
    const bool isHorizontal = m_orientation == Qt::Horizontal;

    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            QRectF rect = qskItemGeometry( item );

            if ( m_isDirty )
            {
                const int index = ( i == 0 ) ? startIndex() : endIndex();
                rect = stackBox->geometryForItemAt( index );

                m_itemOffset[ i ] = isHorizontal ? rect.x() : rect.y();
            }

            qreal x, y;

            if ( isHorizontal )
            {
                qreal off = stackBox->width() * ( value - i );
                if ( m_direction == Qsk::LeftToRight )
                    off = -off;

                x = m_itemOffset[ i ] + off;
                y = rect.y();
            }
            else
            {
                qreal off = stackBox->height() * ( value - i );
                if ( m_direction == Qsk::BottomToTop )
                    off = -off;

                x = rect.x();
                y = m_itemOffset[ i ] + off;
            }

            qskSetItemGeometry( item, x, y, rect.width(), rect.height() );

            if ( !item->isVisible() )
                item->setVisible( true );
        }
    }

    m_isDirty = false;
}

void QskStackBoxAnimator1::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            item->removeEventFilter( this );
            item->setVisible( i == 1 );
        }
    }

    if ( !m_hasClip )
        stackBox()->setClip( false );
}

bool QskStackBoxAnimator1::eventFilter( QObject* object, QEvent* event )
{
    if ( !m_isDirty && object == stackBox() )
    {
        switch( static_cast< int >( event->type() ) )
        {
            case QskEvent::GeometryChange:
            case QskEvent::ContentsRectChange:
            case QskEvent::LayoutRequest:
            {
                m_isDirty = true;
                break;
            }
        }
    }

    return QObject::eventFilter( object, event );
}

QskStackBoxAnimator3::QskStackBoxAnimator3( QskStackBox* parent )
    : QskStackBoxAnimator( parent )
{
}

QskStackBoxAnimator3::~QskStackBoxAnimator3()
{
}

void QskStackBoxAnimator3::setup()
{
    if ( auto item = itemAt( 1 ) )
    {
        item->setOpacity( 0.0 );
        item->setVisible( true );
    }
}

void QskStackBoxAnimator3::advance( qreal value )
{
    if ( auto item1 = itemAt( 0 ) )
        item1->setOpacity( 1.0 - value );

    if ( auto item2 = itemAt( 1 ) )
        item2->setOpacity( value );
}

void QskStackBoxAnimator3::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            item->setOpacity( 1.0 );
            item->setVisible( i == 1 ); // not here !!
        }
    }
}

#include "moc_QskStackBoxAnimator.cpp"
