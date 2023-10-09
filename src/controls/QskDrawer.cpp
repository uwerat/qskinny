/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskDrawer.h"
#include "QskAspect.h"
#include "QskAnimationHint.h"
#include "QskQuick.h"
#include "QskEvent.h"

#include "QskPanGestureRecognizer.h"
#include "QskGesture.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

// we need a skinlet to draw the panel TODO ...
QSK_SUBCONTROL( QskDrawer, Panel )

static QRectF qskDrawerRect( const QRectF& rect,
    Qt::Edge edge, qreal pos, const QSizeF& size )
{
    QRectF r( 0.0, 0.0, size.width(), size.height() );

    const auto progress = pos = 1.0 - pos;

    switch( edge )
    {
        case Qt::LeftEdge:
        {
            r.moveRight( rect.left() + progress * size.width() );
            break;
        }
        case Qt::RightEdge:
        {
            r.moveLeft( rect.right() - progress * size.width() );
            break;
        }

        case Qt::TopEdge:
        {
            r.moveBottom( rect.top() + progress * size.height() );
            break;
        }

        case Qt::BottomEdge:
        {
            r.moveTop( rect.bottom() - progress * size.height() );
            break;
        }
    }

    return r;
}

namespace
{
    class GeometryListener final : public QQuickItemChangeListener
    {
      public:
        GeometryListener( QskDrawer* drawer )
            : m_drawer( drawer )
            , m_parent( drawer->parentItem() )
        {
            setEnabled( true );
        }

        ~GeometryListener()
        {
            setEnabled( false );
        }

      private:
        void itemGeometryChanged( QQuickItem*, 
            QQuickGeometryChange, const QRectF& ) override
        {
            m_drawer->polish();
        }

      private:
        void setEnabled( bool on )
        {
            if ( m_parent )
            {
                const auto changeTypes = QQuickItemPrivate::Geometry;

                auto d = QQuickItemPrivate::get( m_parent );
                if ( on )
                    d->addItemChangeListener( this, changeTypes );
                else
                    d->removeItemChangeListener( this, changeTypes );
            }
        }

        QskDrawer* m_drawer = nullptr;
        QQuickItem* m_parent = nullptr;
    };

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
        QRectF gestureRect() const override
        {
            auto drawer = qobject_cast< QskDrawer* >( parent() );

            const auto dist = 50;
            auto rect = qskItemRect( watchedItem() );

            switch( drawer->edge() )
            {
                case Qt::LeftEdge:
                    rect.setRight( rect.left() + dist );
                    break;

                case Qt::RightEdge:
                    rect.setLeft( rect.right() - dist );
                    break;

                case Qt::TopEdge:
                    rect.setBottom( rect.top() + dist );
                    break;

                case Qt::BottomEdge:
                    rect.setTop( rect.bottom() - dist );
                    break;
            }

            return rect;
        }
    };
}

class QskDrawer::PrivateData
{
  public:
    Qt::Edge edge = Qt::LeftEdge;
    GeometryListener* listener = nullptr;
};

QskDrawer::QskDrawer( QQuickItem* parentItem )
    : Inherited ( parentItem )
    , m_data( new PrivateData )
{
    setZ( 1 );

    setAutoLayoutChildren( true );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );
    setFaderAspect( Panel | QskAspect::Position | QskAspect::Metric );

    connect( this, &QskDrawer::closed,
        this, [this]() { startFading( false ); } );

    /*
        The drawer wants to be on top of the parent - not being
        layouted into its layoutRect(). So we opt out and do
        the layout updates manually.
     */
    setPlacementPolicy( QskPlacementPolicy::Ignore );
    if ( parentItem )
    {
        /*
            QskPopup has an internal QskInputGrabber, that does something
            very similar. Maybe we can make use of it ... TODO
         */
        m_data->listener = new GeometryListener( this );
    }

    (void) new GestureRecognizer( this );
#if 1
    parentItem->setAcceptedMouseButtons( Qt::LeftButton );
#endif
}

QskDrawer::~QskDrawer()
{
    delete m_data->listener;
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

void QskDrawer::gestureEvent( QskGestureEvent* event )
{
    if ( event->gesture()->type() == QskGesture::Pan )
    {
        const auto gesture = static_cast< const QskPanGesture* >( event->gesture().get() );
        if ( gesture->state() == QskGesture::Finished )
            open();

        return;
    }

    Inherited::gestureEvent( event );
}

QRectF QskDrawer::layoutRectForSize( const QSizeF& size ) const
{
    return Inherited::layoutRectForSize( size );
}

void QskDrawer::updateLayout()
{
    if ( !( isOpen() || isFading() ) )
        return;

    const auto targetRect = qskItemRect( parentItem() );
    const auto size = qskConstrainedItemSize( this, targetRect.size() );

    const auto rect = qskDrawerRect( targetRect,
        m_data->edge, metric( faderAspect() ), size );

    qskSetItemGeometry( this, rect );
    Inherited::updateLayout();
}

void QskDrawer::aboutToShow()
{
    startFading( true );
    Inherited::aboutToShow();
}

void QskDrawer::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    if ( change == QQuickItem::ItemParentHasChanged )
    {
        delete m_data->listener;
        m_data->listener = nullptr;

        if ( parentItem() )
            m_data->listener = new GeometryListener( this );
    }
}

void QskDrawer::startFading( bool open )
{
    const auto from = open ? 1.0 : 0.0;
    const auto to = open ? 0.0 : 1.0;

    const auto hint = animationHint( Panel | QskAspect::Position );
    startTransition( faderAspect(), hint, from, to );
}

#include "moc_QskDrawer.cpp"
