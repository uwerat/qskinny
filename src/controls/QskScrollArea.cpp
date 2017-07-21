/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollArea.h"
#include "QskScrollViewSkinlet.h"
#include "QskLayoutConstraint.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickclipnode_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

static QSizeF qskAdjustedSize( const QQuickItem* item, const QSizeF& targetSize )
{
    using namespace QskLayoutConstraint;

    QSizeF sz = effectiveConstraint( item, Qt::PreferredSize );

    qreal w = sz.width();
    qreal h = sz.height();

    if ( targetSize != sz )
    {
        const QSizeF minSize = effectiveConstraint( item, Qt::MinimumSize );
        const QSizeF maxSize = effectiveConstraint( item, Qt::MaximumSize );

        const auto policy = sizePolicy( item );

        if ( targetSize.width() > w )
        {
            if ( policy.horizontalPolicy() & QskSizePolicy::GrowFlag )
                w = qMin( maxSize.width(), targetSize.width() );
        }
        else if ( targetSize.width() < w )
        {
            if ( policy.horizontalPolicy() & QskSizePolicy::ShrinkFlag )
                w = qMax( minSize.width(), w );
        }

        if ( targetSize.height() > h )
        {
            if ( policy.verticalPolicy() & QskSizePolicy::GrowFlag )
                h = qMin( maxSize.height(), targetSize.height() );
        }
        else if ( targetSize.height() < h )
        {
            if ( policy.verticalPolicy() & QskSizePolicy::ShrinkFlag )
                h = qMax( minSize.height(), h );
        }
    }

    return QSizeF( w, h );
}

namespace
{
    class ViewportClipNode final : public QQuickDefaultClipNode
    {
    public:
        ViewportClipNode():
            QQuickDefaultClipNode( QRectF() )
        {
            setGeometry( nullptr );

            // clip nodes have no material, so this flag
            // is available to indicate our replaced clip node

            setFlag( QSGNode::OwnsMaterial, true );
        }

        void copyFrom( const QSGClipNode* other )
        {
            bool isDirty = false;

            if ( other == nullptr )
            {
                if ( !isRectangular() && clipRect().isEmpty() )
                {
                    setIsRectangular( true );
                    setClipRect( QRectF() );
                    setGeometry( nullptr );

                    isDirty = true;
                }
            }
            else
            {
                const bool isRect = other->isRectangular();

                if ( isRect != isRectangular() )
                {
                    setIsRectangular( isRect );
                    isDirty = true;
                }

                if ( clipRect() != other->clipRect() )
                {
                    setClipRect( other->clipRect() );
                    isDirty = true;
                }

                if ( isRect )
                {
                    setGeometry( nullptr );
                }
                else
                {
                    if ( geometry() != other->geometry() )
                    {
                        /*
                            A deep copy would be less efficient, but avoids
                            any further problems - let's see how stable it is.
                         */
                        setGeometry( const_cast< QSGGeometry* >( other->geometry() ) );
                        isDirty = true;
                    }
                }
            }

            if ( isDirty )
                markDirty( QSGNode::DirtyGeometry );
        }

        virtual void update() override final
        {
            /*
                The Qt-Quick framework is limited to setting clipNodes from
                the bounding rectangle. As we need a different clipping
                we turn any updates of the clip done by QQuickWindow
                into nops.
             */
        }
    };
}

class QskScrollAreaClipItem final : public QskControl, public QQuickItemChangeListener
{
    // when inheriting from QskControl we participate in node cleanups
    using Inherited = QskControl;

public:
    QskScrollAreaClipItem( QskScrollArea* );
    virtual ~QskScrollAreaClipItem();

    void enableGeometryListener( bool on );

    QQuickItem* scrolledItem() const
    {
        auto children = childItems();
        return children.isEmpty() ? nullptr : children.first();
    }

protected:
    virtual bool event( QEvent* event ) override final;
    virtual bool childMouseEventFilter( QQuickItem*, QEvent* ) override final;

    virtual void itemChange( ItemChange, const ItemChangeData& ) override final;
    virtual void geometryChanged( const QRectF&, const QRectF& ) override final;

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    virtual void itemGeometryChanged( QQuickItem*,
        QQuickGeometryChange change, const QRectF& ) override final
    {
        if ( change.sizeChange() )
            scrollArea()->polish();
    }

#else
    virtual void itemGeometryChanged( QQuickItem *,
        const QRectF& newRect, const QRectF& oldRect ) override final
    {
        if ( oldRect.size() != newRect.size() )
            scrollArea()->polish();
    }
#endif

    virtual void updateNode( QSGNode* ) override final;

private:
    inline QskScrollArea* scrollArea()
    {
        return static_cast< QskScrollArea* >( parentItem() );
    }

    inline const QskScrollArea* scrollArea() const
    {
        return static_cast< const QskScrollArea* >( parentItem() );
    }

    const QSGClipNode* viewPortClipNode() const;
};

QskScrollAreaClipItem::QskScrollAreaClipItem( QskScrollArea* scrollArea ):
    Inherited( scrollArea )
{
    setObjectName( QStringLiteral( "QskScrollAreaClipItem" ) );

    setClip( true );
    setFiltersChildMouseEvents( true );
}

QskScrollAreaClipItem::~QskScrollAreaClipItem()
{
    enableGeometryListener( false );
}

void QskScrollAreaClipItem::updateNode( QSGNode* )
{
    auto* d = QQuickItemPrivate::get( this );

    if ( QQuickItemPrivate::get( scrollArea() )->dirtyAttributes
        & QQuickItemPrivate::ContentUpdateMask )
    {
        /*
            The update order depends on who calls update first and we
            have to handle being called before a new clip node has
            been created by the scrollview.
            But better invalidate the unguarded clip geometry until then ...
         */
        auto clipNode = d->clipNode();
        if ( clipNode && !clipNode->isRectangular() )
        {
            clipNode->setIsRectangular( true );
            clipNode->setGeometry( nullptr );
        }

        // in the next cycle we will find a valid clip
        update();
        return;
    }

    auto clipNode = d->clipNode();

    if ( clipNode && !( clipNode->flags() & QSGNode::OwnsMaterial ) )
    {
        auto parentNode = clipNode->parent();

        auto node = new ViewportClipNode();
        parentNode->appendChildNode( node );
        clipNode->reparentChildNodesTo( node );

        parentNode->removeChildNode( clipNode );

        if ( clipNode->flags() & QSGNode::OwnedByParent )
            delete clipNode;

        d->extra->clipNode = clipNode = node;
        Q_ASSERT( clipNode == QQuickItemPrivate::get( this )->clipNode() );
    }

    if ( clipNode )
    {
        auto viewClipNode = static_cast< ViewportClipNode* >( clipNode );
        viewClipNode->copyFrom( viewPortClipNode() );
    }
}

const QSGClipNode* QskScrollAreaClipItem::viewPortClipNode() const
{
    auto node = const_cast< QSGNode* >( QskControl::paintNode( scrollArea() ) );
    if ( node )
        node = QskSkinlet::findNodeByRole( node, QskScrollViewSkinlet::ContentsRootRole );

    if ( node && node->type() == QSGNode::ClipNodeType )
        return static_cast< QSGClipNode* >( node );

    return nullptr;
}

void QskScrollAreaClipItem::geometryChanged(
    const QRectF& newRect, const QRectF& oldRect )
{
    Inherited::geometryChanged( newRect, oldRect );

    if ( newRect.size() != oldRect.size() )
    {
        // we need to restore the clip node
        update();
    }
}

void QskScrollAreaClipItem::itemChange(
    QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value )
{
    /*
        Unfortunately QT/Quick developers didn't use the well established
        concept of events and introduced a proprietory and less powerful
        notification system of hooks and optional listeners.
        As we like to support items not being derived from QskControl we have
        to use them.
     */

    if ( ( change == QQuickItem::ItemChildAddedChange ) ||
        ( change == QQuickItem::ItemChildRemovedChange ) )
    {
        /*
           In case we are not adjusting the geometry of the scrolled item
           we have to adjust the scrollview to the geometry changes indicated
           from the scrolled item.
         */

        if ( !scrollArea()->isItemResizable() )
            enableGeometryListener( change == QQuickItem::ItemChildAddedChange );
    }

    Inherited::itemChange( change, value );
}

void QskScrollAreaClipItem::enableGeometryListener( bool on )
{
    auto item = scrolledItem();
    if ( item )
    {
        // we might also be interested in ImplicitWidth/ImplicitHeight
        const QQuickItemPrivate::ChangeTypes types = QQuickItemPrivate::Geometry;

        QQuickItemPrivate* p = QQuickItemPrivate::get( item );
        if ( on )
            p->addItemChangeListener( this, types );
        else
            p->removeItemChangeListener( this, types );
    }
}

bool QskScrollAreaClipItem::event( QEvent* event )
{
    if( event->type() == QEvent::LayoutRequest )
    {
        if ( scrollArea()->isItemResizable() )
            scrollArea()->polish();
    }

    return Inherited::event( event );
}

bool QskScrollAreaClipItem::childMouseEventFilter( QQuickItem* item, QEvent* event )
{
    if ( ( event->type() == QEvent::MouseButtonPress )
        || ( event->type() == QEvent::MouseMove ) )
    {
        const auto mouseEvent = static_cast< const QMouseEvent* >( event );
        const QPointF pos = mapFromScene( mouseEvent->windowPos() );

        auto clipNode = QQuickItemPrivate::get( this )->clipNode();
        if ( clipNode && !clipNode->clipRect().contains( pos ) )
        {
            if ( event->type() == QEvent::MouseButtonPress )
            {
                QCoreApplication::sendEvent( scrollArea(), event );
                return true;
            }
            else if ( event->type() == QEvent::MouseMove )
            {
                if ( item == window()->mouseGrabberItem() )
                    item->ungrabMouse();

                return true;
            }
        }
    }

    return false;
}

class QskScrollArea::PrivateData
{
public:
    PrivateData():
        isItemResizable( true )
    {
    }

    void enableAutoTranslation( QskScrollArea* scrollArea, bool on )
    {
        if ( on )
        {
            QObject::connect( scrollArea, &QskScrollView::scrollPosChanged,
                scrollArea, &QskScrollArea::translateItem );
        }
        else
        {
            QObject::disconnect( scrollArea, &QskScrollView::scrollPosChanged,
                scrollArea, &QskScrollArea::translateItem );
        }
    }

    QskScrollAreaClipItem* clipItem;
    bool isItemResizable : 1;
};

QskScrollArea::QskScrollArea( QQuickItem* parentItem ):
    Inherited( parentItem ),
    m_data( new PrivateData() )
{
    setPolishOnResize( true );
    setFiltersChildMouseEvents( true );

    m_data->clipItem = new QskScrollAreaClipItem( this );
    m_data->enableAutoTranslation( this, true );
}

QskScrollArea::~QskScrollArea()
{
}

void QskScrollArea::updateLayout()
{
    Inherited::updateLayout();

    // the clipItem always has the same geometry as the scroll area
    m_data->clipItem->setSize( size() );
    adjustItem();
}

void QskScrollArea::adjustItem()
{
    QQuickItem* item = m_data->clipItem->scrolledItem();
    if ( item == nullptr )
    {
        setScrollableSize( QSizeF() );
        setScrollPos( QPointF() );
    }
    else
    {
        if ( m_data->isItemResizable )
        {
            const QRectF rect = viewContentsRect();

#if 0
            /*
                For optional scrollbars the available space also depends
                on wether the adjustedSize results in scroll bars. For the
                moment we ignore this and start with a simplified code.
             */
#endif
            auto newSize = qskAdjustedSize( item, rect.size() );
            item->setSize( newSize );
        }

        m_data->enableAutoTranslation( this, false );

        setScrollableSize( QSizeF( item->width(), item->height() ) );
        setScrollPos( scrollPos() );

        m_data->enableAutoTranslation( this, true );

        translateItem();
    }
}

void QskScrollArea::setItemResizable( bool on )
{
    if ( on != m_data->isItemResizable )
    {
        m_data->isItemResizable = on;
        m_data->clipItem->enableGeometryListener( !on );

        Q_EMIT itemResizableChanged();

        if ( m_data->isItemResizable )
            polish();
    }
}

bool QskScrollArea::isItemResizable() const
{
    return m_data->isItemResizable;
}

void QskScrollArea::setScrolledItem( QQuickItem* item )
{
    auto oldItem = m_data->clipItem->scrolledItem();
    if ( item == oldItem )
        return;

    if ( oldItem )
    {
        if ( oldItem->parent() == this )
            delete oldItem;
        else
            oldItem->setParentItem( nullptr );
    }

    if ( item )
    {
        item->setParentItem( m_data->clipItem );
        if ( item->parent() == nullptr )
            item->setParent( m_data->clipItem );
    }

    polish();
    Q_EMIT scrolledItemChanged();
}

QQuickItem* QskScrollArea::scrolledItem() const
{
    return m_data->clipItem->scrolledItem();
}

void QskScrollArea::translateItem()
{
    auto item = m_data->clipItem->scrolledItem();
    if ( item )
    {
        const QPointF pos = viewContentsRect().topLeft() - scrollPos();
        item->setPosition( pos );
    }
}

#include "moc_QskScrollArea.cpp"
