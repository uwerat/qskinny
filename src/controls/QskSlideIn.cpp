/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSlideIn.h"
#include "QskAnimationHint.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

static QPointF qskSlideInTranslation( const QskSlideIn* slideIn )
{
    const auto ratio = 1.0 - slideIn->transitioningFactor();

    auto dx = 0.0;
    auto dy = 0.0;

    switch( slideIn->edge() )
    {
        case Qt::LeftEdge:
            dx = -ratio * slideIn->width();
            break;

        case Qt::RightEdge:
            dx = ratio * slideIn->width();
            break;

        case Qt::TopEdge:
            dy = -ratio * slideIn->height();
            break;

        case Qt::BottomEdge:
            dy = ratio * slideIn->height();
            break;
    }

    return QPointF( dx, dy );
}

static inline QRectF qskUnboundedClipRect( const QRectF& rect, Qt::Edge edge )
{
    /*
        When sliding we want to clip against the edge, where the drawer
        slides in/out. However the size of the slidein is often smaller than the
        one of the parent and we would clip the overlay node
        and all content, that is located outside the drawer geometry.

        So we expand the clip rectangle to "unbounded" at the other edges.
     */
    constexpr qreal d = 1e6;

    QRectF r( -d, -d, 2.0 * d, 2.0 * d );

    switch( edge )
    {
        case Qt::LeftEdge:
            r.setLeft( rect.left() );
            break;

        case Qt::RightEdge:
            r.setRight( rect.right() );
            break;

        case Qt::TopEdge:
            r.setTop( rect.top() );
            break;

        case Qt::BottomEdge:
            r.setBottom( rect.bottom() );
            break;
    }

    return r;
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
            QEvent event( QEvent::PolishRequest );
            QCoreApplication::sendEvent( m_adjustedItem, &event );
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

class QskSlideIn::PrivateData
{
  public:
    inline void resetListener( QskSlideIn* slideIn )
    {
        delete listener;
        listener = nullptr;

        if ( adjustingToParentGeometry )
        {
            if ( slideIn->parentItem() && slideIn->isVisible() )
                listener = new GeometryListener( slideIn->parentItem(), slideIn );
        }
    }

    bool adjustingToParentGeometry = false;
    GeometryListener* listener = nullptr;
};

QskSlideIn::QskSlideIn( QQuickItem* parentItem )
    : Inherited ( parentItem )
    , m_data( new PrivateData )
{
    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    /*
        A slider wants to be on top of its parent - not being
        layouted into its layoutRect(). So we opt out and do
        the layout updates manually.
     */
    setPlacementPolicy( QskPlacementPolicy::Ignore );

    connect( this, &QskPopup::transitioningChanged,
        this, &QskSlideIn::setClip );
}

QskSlideIn::~QskSlideIn()
{
    delete m_data->listener;
}

void QskSlideIn::setAdjustingToParentGeometry( bool on )
{
    if ( on != m_data->adjustingToParentGeometry )
    {
        m_data->adjustingToParentGeometry = on;
        m_data->resetListener( this );
    }
}

bool QskSlideIn::isAdjustingToParentGeometry() const
{
    return m_data->adjustingToParentGeometry;
}

void QskSlideIn::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch( static_cast< int >( change ) )
    {
        case QQuickItem::ItemParentHasChanged:
        case QQuickItem::ItemVisibleHasChanged:
        {
            m_data->resetListener( this );
            break;
        }
    }
}

QRectF QskSlideIn::clipRect() const 
{
    if ( !isTransitioning() )
        return Inherited::clipRect();

    return qskUnboundedClipRect( rect(), edge() );
}

QRectF QskSlideIn::layoutRectForSize( const QSizeF& size ) const
{   
    return QRectF( qskSlideInTranslation( this ), size );
}

#include "moc_QskSlideIn.cpp"
