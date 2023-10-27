/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSlideIn.h"
#include "QskQuick.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

static QPointF qskSlideInTranslation( const QskSlideIn* slideIn, const QSizeF& size )
{
    const auto ratio = 1.0 - slideIn->transitioningFactor();

    auto dx = 0.0;
    auto dy = 0.0;

    switch( slideIn->edge() )
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
        this, &QQuickItem::setClip );
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

void QskSlideIn::updateResources()
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

        r.translate( qskSlideInTranslation( this, r.size() ) );
        setGeometry( r );
    }
}

QRectF QskSlideIn::clipRect() const 
{
    if ( isTransitioning() && parentItem() )
    {
        // parent rectangle translated into the coordinate system of the slideIn
        const QRectF rect( -position(), parentItem()->size() );

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

    return Inherited::clipRect();
}

void QskSlideIn::updateNode( QSGNode* node )
{
    if ( isTransitioning() && clip() )
    {
        if ( auto clipNode = QQuickItemPrivate::get( this )->clipNode() )
        {
            /*
                The clipRect is changing while transitioning. Couldn't
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

#include "moc_QskSlideIn.cpp"
