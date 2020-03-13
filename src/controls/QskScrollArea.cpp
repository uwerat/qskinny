/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollArea.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskScrollViewSkinlet.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickclipnode_p.h>
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

namespace
{
    class ViewportClipNode final : public QQuickDefaultClipNode
    {
      public:
        ViewportClipNode()
            : QQuickDefaultClipNode( QRectF() )
        {
            setGeometry( nullptr );

            // clip nodes have no material, so this flag
            // is available to indicate our replaced clip node

            setFlag( QSGNode::OwnsMaterial, true );
        }

        void copyFrom( const QSGClipNode* other )
        {
            if ( other == nullptr )
            {
                if ( !( isRectangular() && clipRect().isEmpty() ) )
                {
                    setIsRectangular( true );
                    setClipRect( QRectF() );
                    setGeometry( nullptr );

                    markDirty( QSGNode::DirtyGeometry );
                }

                return;
            }

            bool isDirty = false;

            if ( clipRect() != other->clipRect() )
            {
                setClipRect( other->clipRect() );
                isDirty = true;
            }

            if ( other->isRectangular() )
            {
                if ( !isRectangular() )
                {
                    setIsRectangular( true );
                    setGeometry( nullptr );

                    isDirty = true;
                }
            }
            else
            {
                if ( isRectangular() )
                {
                    setIsRectangular( false );
                    isDirty = true;
                }

                if ( geometry() != other->geometry() )
                {
                    // both nodes share the same geometry
                    setGeometry( const_cast< QSGGeometry* >( other->geometry() ) );
                    isDirty = true;
                }
            }

            if ( isDirty )
                markDirty( QSGNode::DirtyGeometry );
        }

        void update() override
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

    bool contains( const QPointF& pos ) const override
    {
        return clipRect().contains( pos );
    }

    QRectF clipRect() const override
    {
        return scrollArea()->subControlRect( QskScrollView::Viewport );
    }

    bool hasFocusIndicatorClip() const override
    {
        return scrollArea()->hasFocusIndicatorClip();
    }

  protected:
    bool event( QEvent* event ) override;

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void geometryChanged( const QRectF&, const QRectF& ) override;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
    void itemGeometryChanged( QQuickItem*,
        QQuickGeometryChange change, const QRectF& ) override
    {
        if ( change.sizeChange() )
            scrollArea()->polish();
    }

#else
    void itemGeometryChanged( QQuickItem*,
        const QRectF& newRect, const QRectF& oldRect ) override
    {
        if ( oldRect.size() != newRect.size() )
            scrollArea()->polish();
    }
#endif

    void updateNode( QSGNode* ) override;

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

QskScrollAreaClipItem::QskScrollAreaClipItem( QskScrollArea* scrollArea )
    : Inherited( scrollArea )
{
    setObjectName( QStringLiteral( "QskScrollAreaClipItem" ) );
    setClip( true );
}

QskScrollAreaClipItem::~QskScrollAreaClipItem()
{
    enableGeometryListener( false );
}

void QskScrollAreaClipItem::updateNode( QSGNode* )
{
    auto* d = QQuickItemPrivate::get( this );

    if ( QQuickItemPrivate::get( scrollArea() )->dirtyAttributes &
        QQuickItemPrivate::ContentUpdateMask )
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
        // Replace the clip node being inserted from QQuickWindow

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
        /*
            Update the clip node with the geometry of the clip node
            of the viewport of the scrollview.
         */
        auto viewClipNode = static_cast< ViewportClipNode* >( clipNode );
        viewClipNode->copyFrom( viewPortClipNode() );
    }
}

const QSGClipNode* QskScrollAreaClipItem::viewPortClipNode() const
{
    auto node = const_cast< QSGNode* >( qskPaintNode( scrollArea() ) );
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
    if ( change == QQuickItem::ItemChildAddedChange )
    {
        enableGeometryListener( true );
    }
    else if ( change == QQuickItem::ItemChildRemovedChange )
    {
        enableGeometryListener( false );
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
    if ( event->type() == QEvent::LayoutRequest )
    {
        if ( scrollArea()->isItemResizable() )
            scrollArea()->polish();
    }

    return Inherited::event( event );
}

class QskScrollArea::PrivateData
{
  public:
    PrivateData()
        : isItemResizable( true )
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

    QskScrollAreaClipItem* clipItem = nullptr;

    bool isItemResizable : 1;
};

/*
    When doing scene graph composition it is quite easy to insert a clip node
    somewhere below the paint node to have all items on the viewport being clipped.
    This is how it is done f.e. for the list boxes.

    But when having QQuickItems on the viewport we run into a fundamental limitation
    of the Qt/Quick design: node subtrees for the children have to be in parallel to
    the paint node.

    We work around this problem, by inserting an extra item between the scroll area
    and the scrollable item. This item replaces its default clip node by its own node,
    that references the geometry of the viewport clip node.
 */

QskScrollArea::QskScrollArea( QQuickItem* parentItem )
    : Inherited( parentItem )
    , m_data( new PrivateData() )
{
    setPolishOnResize( true );

    m_data->clipItem = new QskScrollAreaClipItem( this );
    m_data->enableAutoTranslation( this, true );
}

QskScrollArea::~QskScrollArea()
{
    delete m_data->clipItem;
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
    auto item = m_data->clipItem->scrolledItem();

    if ( item == nullptr )
    {
        setScrollableSize( QSizeF() );
        setScrollPos( QPointF() );
    }
    else
    {
        if ( m_data->isItemResizable )
        {
            auto size = viewContentsRect().size();

#if 0
            /*
                For optional scrollbars the available space also depends
                on wether the adjustedSize results in scroll bars. For the
                moment we ignore this and start with a simplified code.
             */
#endif
            size = qskConstrainedItemSize( item, size );
            item->setSize( size );
        }

        m_data->enableAutoTranslation( this, false );

        setScrollableSize( QSizeF( item->width(), item->height() ) );
        setScrollPos( scrollPos() );

        m_data->enableAutoTranslation( this, true );

        translateItem();
    }
}

bool QskScrollArea::hasFocusIndicatorClip() const
{
    return true;
}

void QskScrollArea::setItemResizable( bool on )
{
    if ( on != m_data->isItemResizable )
    {
        m_data->isItemResizable = on;
        Q_EMIT itemResizableChanged( on );

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
    if ( auto item = m_data->clipItem->scrolledItem() )
    {
        const QPointF pos = viewContentsRect().topLeft() - scrollPos();
        item->setPosition( pos );
    }
}

#include "moc_QskScrollArea.cpp"
