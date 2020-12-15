/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollArea.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskScrollViewSkinlet.h"
#include "QskBoxBorderMetrics.h"
#include "QskSGNode.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickclipnode_p.h>
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

static inline bool qskNeedsScrollBars(
    qreal available, qreal required, Qt::ScrollBarPolicy policy )
{
    if ( policy == Qt::ScrollBarAsNeeded )
        return required > available;
    else
        return policy == Qt::ScrollBarAlwaysOn;
}

static inline QSizeF qskPanelInnerSize( const QskScrollView* scrollView )
{
    auto size = scrollView->subControlRect( QskScrollView::Panel ).size();

    const auto borderMetrics = scrollView->boxBorderMetricsHint( QskScrollView::Viewport );
    const qreal bw = 2 * borderMetrics.widthAt( Qt::TopEdge );

    size.setWidth( qMax( size.width() - bw, 0.0 ) );
    size.setHeight( qMax( size.height() - bw, 0.0 ) );

    return size;
}

static inline QSizeF qskScrolledItemSize( const QskScrollView* scrollView,
    const QQuickItem* item, const QSizeF& boundingSize )
{
    using Q = QskScrollView;

    QSizeF outerSize = boundingSize;

    const qreal spacing = scrollView->spacingHint( Q::Panel );

    const auto sbV = scrollView->metric( Q::VerticalScrollBar | QskAspect::Size );
    const auto sbH = scrollView->metric( Q::HorizontalScrollBar | QskAspect::Size );

    const auto policyH = scrollView->horizontalScrollBarPolicy();
    const auto policyV = scrollView->verticalScrollBarPolicy();

    auto itemSize = qskConstrainedItemSize( item, outerSize );

    bool needScrollBarV = qskNeedsScrollBars( outerSize.height(), itemSize.height(), policyV );
    bool needScrollBarH = qskNeedsScrollBars( outerSize.width(), itemSize.width(), policyH );

    bool hasScrollBarV = needScrollBarV;

    // Vertical/Horizonal scroll bars might depend on each other

    if ( needScrollBarV )
    {
        outerSize.rwidth() -= sbV + spacing;
        itemSize = qskConstrainedItemSize( item, outerSize );

        if ( !needScrollBarH )
        {
            needScrollBarH = qskNeedsScrollBars(
                outerSize.width(), itemSize.width(), policyH );
        }
    }

    if ( needScrollBarH )
    {
        outerSize.rheight() -= sbH + spacing;
        itemSize = qskConstrainedItemSize( item, outerSize );

        if ( !hasScrollBarV )
        {
            needScrollBarV = qskNeedsScrollBars(
                outerSize.height(), itemSize.height(), policyV );
        }
    }

    if ( needScrollBarV )
    {
        outerSize.rwidth() -= sbV + spacing;
        itemSize = qskConstrainedItemSize( item, outerSize );
    }

    return itemSize;
}

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

namespace
{
    class ClipItem final : public QskControl, public QQuickItemChangeListener
    {
        // when inheriting from QskControl we participate in node cleanups
        using Inherited = QskControl;

      public:
        ClipItem( QskScrollArea* );
        virtual ~ClipItem();

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

        inline void setItemSizeChangedEnabled( bool on )
        {
            m_isSizeChangedEnabled = on;
        }

      protected:
        bool event( QEvent* event ) override;

        void itemChange( ItemChange, const ItemChangeData& ) override;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
        void itemGeometryChanged( QQuickItem*,
            QQuickGeometryChange change, const QRectF& ) override
        {
            if ( m_isSizeChangedEnabled && change.sizeChange() )
                scrollArea()->polish();
        }

#else
        void itemGeometryChanged( QQuickItem*,
            const QRectF& newRect, const QRectF& oldRect ) override
        {
            if ( m_isSizeChangedEnabled && ( oldRect.size() != newRect.size() ) )
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

        bool m_isSizeChangedEnabled = true;
    };

    ClipItem::ClipItem( QskScrollArea* scrollArea )
        : Inherited( scrollArea )
    {
        setObjectName( QStringLiteral( "QskScrollAreaClipItem" ) );
        setClip( true );
    }

    ClipItem::~ClipItem()
    {
        enableGeometryListener( false );
    }

    void ClipItem::updateNode( QSGNode* )
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

    const QSGClipNode* ClipItem::viewPortClipNode() const
    {
        auto node = const_cast< QSGNode* >( qskPaintNode( scrollArea() ) );
        if ( node )
            node = QskSGNode::findChildNode( node, QskScrollViewSkinlet::ContentsRootRole );

        if ( node && node->type() == QSGNode::ClipNodeType )
            return static_cast< QSGClipNode* >( node );

        return nullptr;
    }

    void ClipItem::itemChange(
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

    void ClipItem::enableGeometryListener( bool on )
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

    bool ClipItem::event( QEvent* event )
    {
        const int eventType = event->type();

        if ( eventType == QEvent::LayoutRequest )
        {
            if ( scrollArea()->isItemResizable() )
                scrollArea()->polish();
        }
        else if ( eventType == QskEvent::GeometryChange )
        {
            auto geometryEvent = static_cast< const QskGeometryChangeEvent* >( event );
            if ( geometryEvent->isResized() )
            {
                // we need to restore the clip node
                update();
            }
        }

        return Inherited::event( event );
    }
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

    ClipItem* clipItem = nullptr;

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

    m_data->clipItem = new ClipItem( this );
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

        return;
    }

    if ( m_data->isItemResizable )
    {
        QSizeF itemSize;

        const auto viewSize = qskPanelInnerSize( this );
        if ( !viewSize.isEmpty() )
        {
            // we have to anticipate the scrollbars
            itemSize = qskScrolledItemSize( this, item, viewSize );
        }

        if ( itemSize.isEmpty() )
            itemSize = QSizeF( 0.0, 0.0 );


        m_data->clipItem->setItemSizeChangedEnabled( false );
        item->setSize( itemSize );
        m_data->clipItem->setItemSizeChangedEnabled( true );
    }

    m_data->enableAutoTranslation( this, false );

    setScrollableSize( QSizeF( item->width(), item->height() ) );
    setScrollPos( scrollPos() );

    m_data->enableAutoTranslation( this, true );

    translateItem();
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
