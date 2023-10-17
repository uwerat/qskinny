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

#include <qguiapplication.h>
#include <qstylehints.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

/*
    Only used for the sliding in animation. Do we want to
    introduce a specific panel as background ???
 */
QSK_SUBCONTROL( QskDrawer, Panel )

static void qskCatchMouseEvents( QQuickItem* item )
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

static void qskLayoutDrawer( const QRectF& rect, QskDrawer* drawer )
{
    const auto size = qskSizeConstraint( drawer, Qt::PreferredSize );

    QRectF r( 0.0, 0.0, size.width(), size.height() );

    switch( drawer->edge() )
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

    drawer->setGeometry( r );
}

static inline QRectF qskSlidingRect(
    const QSizeF& size, Qt::Edge edge, qreal ratio )
{
    auto x = 0.0;
    auto y = 0.0;

    switch( edge )
    {
        case Qt::LeftEdge:
            x = -ratio * size.width();
            break;

        case Qt::RightEdge:
            x = ratio * size.width();
            break;

        case Qt::TopEdge:
            y = -ratio * size.height();
            break;

        case Qt::BottomEdge:
            y = ratio * size.height();
            break;
    }

    return QRectF( x, y, size.width(), size.height() );
}

namespace
{
    class GeometryListener final : public QQuickItemChangeListener
    {
      public:
        GeometryListener( QQuickItem* item, QQuickItem* adjustedItem )
            : m_item( item )
            , m_adjustedItem( adjustedItem )
        {
            adjust();
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
            adjust();
        }

      private:
        void adjust()
        {
#if 0
            const auto pos = m_adjustedItem->mapFromItem( m_item, QPointF() );
            qskSetItemGeometry( m_adjustedItem,
                pos.x(), pos.y(), m_item->width(), m_item->height() );
#else
            qskLayoutDrawer( QRectF( QPointF(), m_item->size() ),
                qobject_cast< QskDrawer* >( m_adjustedItem ) );
#endif
        }

        void setEnabled( bool on )
        {
            const auto changeTypes = QQuickItemPrivate::Geometry;

            auto d = QQuickItemPrivate::get( m_item );
            if ( on )
                d->addItemChangeListener( this, changeTypes );
            else
                d->removeItemChangeListener( this, changeTypes );
        }

        QQuickItem* m_item;
        QQuickItem* m_adjustedItem;
    };
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
            if ( drawer->isFading() )
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
    GeometryListener* listener = nullptr;

    // a skin hint ???
    qreal dragMargin = QGuiApplication::styleHints()->startDragDistance();
};

QskDrawer::QskDrawer( QQuickItem* parentItem )
    : Inherited ( parentItem )
    , m_data( new PrivateData )
{
#if 1
    setZ( 1 );
#endif

    setAutoLayoutChildren( true );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );
    setFaderAspect( Panel | QskAspect::Position | QskAspect::Metric );

    /*
        The drawer wants to be on top of the parent - not being
        layouted into its layoutRect(). So we opt out and do
        the layout updates manually.
     */
    setPlacementPolicy( QskPlacementPolicy::Ignore );
    if ( parentItem )
    {
        m_data->listener = new GeometryListener( parentItem, this );
        qskCatchMouseEvents( parentItem );
    }

    m_data->gestureRecognizer = new GestureRecognizer( this );


    connect( this, &QskPopup::openChanged, this, &QskDrawer::setFading );

    /*
        When the content of the parentItem does not fit we will have
        a difference between fading and normal state. To overcome this problem
        we need to expand the rectangle of the QQuickDefaultClipNode manually to
        the window borders: TODO ...
     */
    connect( this, &QskPopup::fadingChanged, parentItem, &QQuickItem::setClip );
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

void QskDrawer::setDragMargin( qreal margin )
{
    margin = std::max( margin, 0.0 );

    if ( margin != m_data->dragMargin )
    {
        m_data->dragMargin = margin;
        Q_EMIT dragMarginChanged( margin );
    }
}

qreal QskDrawer::dragMargin() const
{
    return m_data->dragMargin;
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

QRectF QskDrawer::layoutRectForSize( const QSizeF& size ) const
{
    qreal ratio;

    if ( isFading() )
        ratio = metric( faderAspect() );
    else
        ratio = isOpen() ? 0.0 : 1.0;

    return qskSlidingRect( size, m_data->edge, ratio );
}

void QskDrawer::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch( static_cast< int >( change ) )
    {
        case QQuickItem::ItemParentHasChanged:
        {
            if ( parentItem() )
                qskCatchMouseEvents( parentItem() );

            Q_FALLTHROUGH();
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            delete m_data->listener;
            m_data->listener = nullptr;

            if ( parentItem() && isVisible() )
                m_data->listener = new GeometryListener( parentItem(), this );

            break;
        }
    }
}

void QskDrawer::setFading( bool on )
{
    const qreal from = on ? 1.0 : 0.0;
    const qreal to = on ? 0.0 : 1.0;

    const auto aspect = faderAspect();

    auto hint = animationHint( aspect );
    hint.updateFlags = QskAnimationHint::UpdatePolish;

    startTransition( aspect, hint, from, to );
}

#include "moc_QskDrawer.cpp"
