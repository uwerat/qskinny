/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskDrawer.h"
#include "QskAspect.h"
#include "QskQuick.h"
#include "QskEvent.h"

#include "QskPanGestureRecognizer.h"
#include "QskGesture.h"

#include <qguiapplication.h>
#include <qstylehints.h>

static inline qreal qskDefaultDragMargin()
{
    // a skin hint ???
    return QGuiApplication::styleHints()->startDragDistance();
}

static inline void qskCatchMouseEvents( QQuickItem* item )
{
#if 1
    // manipulating other items - do we really want to do this ?
    item->setAcceptedMouseButtons( Qt::LeftButton );
    item->setFiltersChildMouseEvents( true );
#endif
}

static bool qskCheckDirection( Qt::Edge edge, const QskPanGesture* gesture )
{
    const auto degrees = gesture->angle();

    switch( edge )
    {
        case Qt::LeftEdge:
            return ( degrees < 90.0 ) || ( degrees ) > 270.0;

        case Qt::RightEdge:
            return ( degrees > 90.0 ) && ( degrees < 270.0 );

        case Qt::TopEdge:
            return degrees > 180.0;

        case Qt::BottomEdge:
            return degrees < 180.0;
    }

    return false;
}

static inline QRectF qskAlignedToEdge(
    const QRectF& rect, const QSizeF& size, Qt::Edge edge )
{
    QRectF r( 0.0, 0.0, size.width(), size.height() );

    switch( edge )
    {
        case Qt::LeftEdge:
        {
            r.setHeight( rect.height() );
            r.moveRight( rect.left() + size.width() );
            break;
        }
        case Qt::RightEdge:
        {
            r.setHeight( rect.height() );
            r.moveLeft( rect.right() - size.width() );
            break;
        }

        case Qt::TopEdge:
        {
            r.setWidth( rect.width() );
            r.moveBottom( rect.top() + size.height() );
            break;
        }

        case Qt::BottomEdge:
        {
            r.setWidth( rect.width() );
            r.moveTop( rect.bottom() - size.height() );
            break;
        }
    }

    return r;
}

namespace
{
    class GestureRecognizer : public QskPanGestureRecognizer
    {
        using Inherited = QskPanGestureRecognizer;

      public:
        GestureRecognizer( QskDrawer* drawer )
            : QskPanGestureRecognizer( drawer )
        {
            setWatchedItem( drawer->parentItem() );
            setTargetItem( drawer );
        }

      protected:
        bool isAcceptedPos( const QPointF& pos ) const override
        {
            auto drawer = qobject_cast< const QskDrawer* >( targetItem() );
            if ( drawer->isTransitioning() )
                return false;

            auto rect = qskItemRect( watchedItem() );

            if ( !drawer->isOpen() )
            {
                const auto dragMargin = drawer->dragMargin();
                if ( dragMargin <= 0.0 )
                    return false;

                switch( drawer->edge() )
                {
                    case Qt::LeftEdge:
                        rect.setRight( rect.left() + dragMargin );
                        break;

                    case Qt::RightEdge:
                        rect.setLeft( rect.right() - dragMargin );
                        break;

                    case Qt::TopEdge:
                        rect.setBottom( rect.top() + dragMargin );
                        break;

                    case Qt::BottomEdge:
                        rect.setTop( rect.bottom() - dragMargin );
                        break;
                }
            }

            return rect.contains( pos );
        }
    };
}

class QskDrawer::PrivateData
{
  public:
    Qt::Edge edge = Qt::LeftEdge;
    GestureRecognizer* gestureRecognizer = nullptr;

    qreal dragMargin = qskDefaultDragMargin();
};

QskDrawer::QskDrawer( QQuickItem* parentItem )
    : Inherited ( parentItem )
    , m_data( new PrivateData )
{
#if 1
    setZ( 1 );
#endif

    setAutoLayoutChildren( true );
    setInteractive( true );
    setAdjustingToParentGeometry( true );
}

QskDrawer::~QskDrawer()
{
}

Qt::Edge QskDrawer::edge() const
{
    return m_data->edge;
}

void QskDrawer::setEdge( Qt::Edge edge )
{
    if( m_data->edge == edge )
        return;

    update();
    m_data->edge = edge;
    edgeChanged( edge );
}

void QskDrawer::setInteractive( bool on )
{
    if ( on == isInteractive() )
        return;

    if ( on )
    {
        m_data->gestureRecognizer = new GestureRecognizer( this );
        if ( parentItem() )
            qskCatchMouseEvents( parentItem() );
    }
    else
    {
        // how to revert qskCatchMouseEvents properly ???
        delete m_data->gestureRecognizer;
        m_data->gestureRecognizer = nullptr;
    }

    Q_EMIT interactiveChanged( on );
}

bool QskDrawer::isInteractive() const
{
    return m_data->gestureRecognizer != nullptr;
}

void QskDrawer::setDragMargin( qreal margin )
{
    margin = std::max( margin, 0.0 );

    if ( margin != m_data->dragMargin )
    {
        m_data->dragMargin = margin;
        Q_EMIT dragMarginChanged( margin );
    }
}

void QskDrawer::resetDragMargin()
{
    setDragMargin( qskDefaultDragMargin() );
}

qreal QskDrawer::dragMargin() const
{
    return m_data->dragMargin;
}

bool QskDrawer::event( QEvent* event )
{
    if ( event->type() == QEvent::PolishRequest )
    {
        if ( isAdjustingToParentGeometry() && parentItem() )
        {
            auto r = qskItemRect( parentItem() );
            r = qskAlignedToEdge( r, sizeConstraint( Qt::PreferredSize ), edge() );

            setGeometry( r );

            return true;
        }
    }

    return Inherited::event( event );
}

void QskDrawer::gestureEvent( QskGestureEvent* event )
{
    if ( event->gesture()->type() == QskGesture::Pan )
    {
        /*
            For the moment we treat the gesture like a swipe gesture
            without dragging the drawer when moving the mouse. TODO ...
         */
        const auto gesture = static_cast< const QskPanGesture* >( event->gesture().get() );
        if ( gesture->state() == QskGesture::Finished )
        {
            const auto forwards = qskCheckDirection( m_data->edge, gesture );
            if ( forwards != isOpen() )
                setOpen( forwards );
        }

        return;
    }

    Inherited::gestureEvent( event );
}

void QskDrawer::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch( static_cast< int >( change ) )
    {
        case QQuickItem::ItemParentHasChanged:
        {
            if ( parentItem() && isInteractive() )
                qskCatchMouseEvents( parentItem() );

            break;
        }
    }
}

#include "moc_QskDrawer.cpp"
