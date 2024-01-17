/******************************************************************************
 * QSkinny - Copyright (C) The authors
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

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskDrawer, Panel )

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
    const QRectF& r, const QSizeF& sz, Qt::Edge edge )
{
    switch( edge )
    {
        case Qt::LeftEdge:
            return QRectF( r.left(), r.top(), sz.width(), r.height() );

        case Qt::RightEdge:
            return QRectF( r.right() - sz.width(), r.top(), sz.width(), r.height() );

        case Qt::TopEdge:
            return QRectF( r.left(), r.top(), r.width(), sz.height() );

        case Qt::BottomEdge:
            return QRectF( r.left(), r.bottom() - sz.height(), r.width(), sz.height() );
    }

    return QRectF();
}

static QPointF qskDrawerTranslation( const QskDrawer* drawer, const QSizeF& size )
{
    const auto ratio = 1.0 - drawer->fadingFactor();

    auto dx = 0.0;
    auto dy = 0.0;

    switch( drawer->edge() )
    {
        case Qt::LeftEdge:
            dx = -ratio * size.width();
            break;

        case Qt::RightEdge:
            dx = ratio * size.width();
            break;

        case Qt::TopEdge:
            dy = -ratio * size.height();
            break;

        case Qt::BottomEdge:
            dy = ratio * size.height();
            break;
    }

    return QPointF( dx, dy );
}

namespace
{
    // Using an eventFilter for QskEvent::GeometryChange instead ???

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
            m_adjustedItem->polish();
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
    PrivateData( Qt::Edge edge )
        : edge( edge )
    {
    }

    inline void resetListener( QskDrawer* drawer )
    {
        delete listener;
        listener = nullptr;

        if ( drawer->parentItem() && drawer->isVisible() )
            listener = new GeometryListener( drawer->parentItem(), drawer );
    }

    GeometryListener* listener = nullptr;
    GestureRecognizer* gestureRecognizer = nullptr;

    qreal dragMargin = qskDefaultDragMargin();
    Qt::Edge edge;
};

QskDrawer::QskDrawer( QQuickItem* parentItem )
    : QskDrawer( Qt::LeftEdge, parentItem )
{
}

QskDrawer::QskDrawer( Qt::Edge edge, QQuickItem* parentItem )
    : Inherited ( parentItem )
    , m_data( new PrivateData( edge ) )
{
#if 1
    setZ( 1 );
#endif

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    /*
        A drawer wants to be on top of its parent - not being
        layouted into its layoutRect(). So we opt out and do
        the layout updates manually.
     */
    setPlacementPolicy( QskPlacementPolicy::Ignore );

    setAutoLayoutChildren( true );
    setInteractive( true );

    connect( this, &QskPopup::fadingChanged, this, &QQuickItem::setClip );
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

            m_data->resetListener( this );
            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            m_data->resetListener( this );
            break;
        }
    }
}

void QskDrawer::updateResources()
{
    Inherited::updateResources();

    /*
         Adjusting the geometry to the parent needs to be done before
         the layouting of the children ( -> autoLayoutChildren ) is done.
         So we are using this hook even if it is not about resources: TODO ...
     */
    if ( const auto item = parentItem() )
    {
        auto r = qskItemRect( item );
        r = qskAlignedToEdge( r, sizeConstraint( Qt::PreferredSize ), edge() );

        r.translate( qskDrawerTranslation( this, r.size() ) );
        setGeometry( r );
    }
}

void QskDrawer::updateNode( QSGNode* node )
{
    if ( isFading() && clip() )
    {
        if ( auto clipNode = QQuickItemPrivate::get( this )->clipNode() )
        {
            /*
                The clipRect is changing while fading. Couldn't
                find a way how to trigger updates - maybe be enabling/disabling
                the clip. So we do the updates manually. TODO ...
             */
            const auto r = clipRect();
            if ( r != clipNode->rect() )
            {
                clipNode->setRect( r );
                clipNode->update();
            }
        }
    }

    Inherited::updateNode( node );
}

QRectF QskDrawer::clipRect() const
{
    if ( isFading() && parentItem() )
    {
        /*
            We might not fit into our parent and our children not
            into our rect. So we want to have a clip against the
            edge, where the drawer slides in/out only.
            Otherwise we would have unwanted effects, when clipping gets
            disabled once the transition is over.
         */
        constexpr qreal d = 1e6;

        QRectF r( -d, -d, 2.0 * d, 2.0 * d );

        switch( edge() )
        {
            case Qt::LeftEdge:
                r.setLeft( -x() );
                break;

            case Qt::RightEdge:
                r.setRight( parentItem()->width() - x() );
                break;

            case Qt::TopEdge:
                r.setTop( -y() );
                break;

            case Qt::BottomEdge:
                r.setBottom( parentItem()->height() - y() );
                break;
        }

        return r;
    }

    return Inherited::clipRect();
}

QskAspect QskDrawer::fadingAspect() const
{
    return QskDrawer::Panel | QskAspect::Position;
}

QRectF QskDrawer::layoutRectForSize( const QSizeF& size ) const
{
    return subControlContentsRect( size, Panel );
}

#include "moc_QskDrawer.cpp"
