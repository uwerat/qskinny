/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotView.h"
#include "QskPlotItem.h"

#include <QskTickmarks.h>
#include <QskGraduation.h>
#include <QskSkinlet.h>

#include <QskQuick.h>
#include <QskBoxClipNode.h>
#include <QskBoxBorderMetrics.h>

#include <qsgnode.h>
#include <vector>

QSK_SUBCONTROL( QskPlotView, Panel )
QSK_SUBCONTROL( QskPlotView, AxisScale )
QSK_SUBCONTROL( QskPlotView, Canvas )

enum { AxisCount =  2 };

static inline bool qskIsXAxis( int axis )
{
    return axis == QskPlot::XBottom;
}

static inline QTransform qskScaleTransform( Qt::Orientation orientation,
    qreal s1, qreal s2, qreal p1, qreal p2 )
{
    const auto ratio = ( p2 - p1 ) / ( s2 - s1 );

    if ( orientation == Qt::Horizontal )
    {
        auto transform = QTransform::fromTranslate( -s1, 0.0 );
        transform *= QTransform::fromScale( ratio, 1.0 );
        transform *= QTransform::fromTranslate( p1, 0.0 );

        return transform;
    }
    else
    {
        auto transform = QTransform::fromTranslate( 0.0, -s1 );
        transform *= QTransform::fromScale( 1.0, -ratio );
        transform *= QTransform::fromTranslate( 0.0, p2 );

        return transform;
    }
}

class QskPlotView::PrivateData
{
  public:

    struct
    {
        QskIntervalF boundaries;
        QskTickmarks tickmarks;

        bool boundariesDirty = true;
        bool ticksDirty = true;

    } scales[ AxisCount ];

    class ItemData
    {
      public:
        QskPlotItem* plotItem = nullptr;
        QSGTransformNode* node = nullptr;
    };

    inline bool needsClipping() const
    {
        for ( const auto& data : itemData )
        {
            if ( data.plotItem->needsClipping() )
                return true;
        }

        return false;
    }

    std::vector< ItemData > itemData; // a flat map
    QVector< QSGNode* > orphanedNodes;

    QRectF canvasRect;
};

QskPlotView::QskPlotView( QQuickItem* parentItem )
    : Inherited( parentItem )
    , m_data( new PrivateData )
{
    setBoundaries( QskPlot::XBottom, 0.0, 100.0 );
    setBoundaries( QskPlot::YLeft, 0.0, 100.0 );
}

QskPlotView::~QskPlotView()
{
}

void QskPlotView::setBoundaries( QskPlot::Axis axis, qreal from, qreal to )
{
    setBoundaries( axis, QskIntervalF( from, to ) );
}

void QskPlotView::setBoundaries( QskPlot::Axis axis, const QskIntervalF& boundaries )
{
    auto& sd = m_data->scales[ axis ];

    if ( boundaries == sd.boundaries )
        return;

    sd.boundaries = boundaries;
    sd.boundariesDirty = true;

    const auto oldTickmarks = sd.tickmarks;

    sd.tickmarks = QskGraduation::divideInterval(
        boundaries.lowerBound(), boundaries.upperBound(), 5, 5 );

    if ( oldTickmarks != sd.tickmarks )
        sd.ticksDirty = true;

    polish();
    update();
}

QskIntervalF QskPlotView::boundaries( QskPlot::Axis axis ) const
{
    return m_data->scales[ axis ].boundaries;
}

QskTickmarks QskPlotView::tickmarks( QskPlot::Axis axis ) const
{
    return m_data->scales[ axis ].tickmarks;
}

QTransform QskPlotView::transformation( QskPlot::Axis xAxis, QskPlot::Axis yAxis ) const
{
    const auto r = canvasRect();

    const auto scales = m_data->scales;

    const qreal x1 = scales[xAxis].boundaries.lowerBound();
    const qreal x2 = scales[xAxis].boundaries.upperBound();

    const qreal y1 = scales[yAxis].boundaries.lowerBound();
    const qreal y2 = scales[yAxis].boundaries.upperBound();

    auto transform = QTransform::fromTranslate( -x1, -y1 );

    transform *= QTransform::fromScale(
        r.width() / ( x2 - x1 ), r.height() / ( y1 - y2 ) );

    transform *= QTransform::fromTranslate( r.left(), r.bottom() );

    return transform;
}

QRectF QskPlotView::canvasRect() const
{
    const auto r = subControlRect( Canvas );

    const auto b = boxBorderMetricsHint( Canvas );
    return r.adjusted( b.left(), b.top(), -b.right(), -b.bottom() );
}

QVariant QskPlotView::labelAt( QskPlot::Axis, qreal pos ) const
{
    return QString::number( pos, 'g' );
}

void QskPlotView::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::StyleChange )
    {
        for ( auto& itemData : m_data->itemData )
        {
            auto plotItem = itemData.plotItem;

            if ( plotItem->skinlet() == nullptr )
                plotItem->setSkinlet( nullptr ); // clearing the cached skinlet

            plotItem->markDirty();
        }

        polish();
    }
}

void QskPlotView::geometryChange( const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChange( newGeometry, oldGeometry );
    polish();
}

void QskPlotView::attachItem( QskPlotItem* plotItem )
{
    const auto cmp = []( const qreal& z, const PrivateData::ItemData& data )
        { return z < data.plotItem->z(); };

    auto& itemData = m_data->itemData;

    auto it = std::upper_bound( itemData.begin(), itemData.end(), plotItem->z(), cmp );
    itemData.insert( it, { plotItem, nullptr } );

    plotItem->markDirty();

    update();
}

void QskPlotView::detachItem( QskPlotItem* plotItem )
{
    auto& itemData = m_data->itemData;

    for ( auto it = itemData.begin(); it != itemData.end(); ++it )
    {
        if ( it->plotItem == plotItem )
        {
            if ( it->node )
                m_data->orphanedNodes += it->node;

            itemData.erase( it );
            update();

            return;
        }
    }
}

void QskPlotView::updateResources()
{
    using namespace QskPlot;
    using I = QskPlotItem;

    /*
        updateResources is called from updatePolish.
        We should find a better name: TODO ...
     */

    I::ChangeFlags flags[ AxisCount ];

    {
        bool canvasChanged = false;

        {
            const auto canvasRect = this->canvasRect();
            if ( canvasRect != m_data->canvasRect )
            {
                m_data->canvasRect = canvasRect;
                canvasChanged = true;
            }
        }

        for ( int axis = 0; axis < AxisCount; axis++ )
        {
            I::ChangeFlags flag;

            auto& scaleData = m_data->scales[ axis ];
            if ( canvasChanged )
                flag |= I::CanvasGeometryChanged;

            if ( scaleData.boundariesDirty )
                flag |= qskIsXAxis( axis ) ? I::XBoundariesChanged : I::YBoundariesChanged;

            if ( scaleData.ticksDirty )
                flag |= qskIsXAxis( axis ) ? I::XTickmarksChanged : I::YTickmarksChanged;

            flags[axis] = flag;
            scaleData.boundariesDirty = scaleData.ticksDirty = false;
        }
    }

    if ( auto itemFlags = ( flags[0] | flags[1] ) )
    {
        for ( auto& itemData : m_data->itemData )
            itemData.plotItem->transformationChanged( itemFlags );
    }
}

void QskPlotView::updateNode( QSGNode* node )
{
    if ( !m_data->orphanedNodes.empty() )
    {
        for ( auto node : m_data->orphanedNodes )
        {
            if ( auto parentNode = node->parent() )
                parentNode->removeChildNode( node );

            delete node;
        }

        m_data->orphanedNodes.clear();
    }

    // sorting items according to z TODO ...

    auto oldItemsNode = node->lastChild();
    if ( oldItemsNode )
        node->removeChildNode( oldItemsNode );

    /*
        the scene graph subtree might have been removed for situations
        like skin changes and the item node pointers are dangling pointers
     */
    const bool danglingNodes = ( oldItemsNode == nullptr );

    Inherited::updateNode( node );

    auto itemsNode = oldItemsNode;

    if ( m_data->needsClipping() )
    {
        if ( itemsNode == nullptr || itemsNode->type() != QSGNode::ClipNodeType )
            itemsNode = new QskBoxClipNode();
    }
    else
    {
        if ( itemsNode == nullptr || itemsNode->type() == QSGNode::ClipNodeType )
            itemsNode = new QSGNode();
    }

    if ( oldItemsNode && itemsNode != oldItemsNode )
    {
        oldItemsNode->reparentChildNodesTo( itemsNode );
        delete oldItemsNode;
    }

    node->appendChildNode( itemsNode );

    if ( itemsNode->type() == QSGNode::ClipNodeType )
    {
        /*
            updating the clip node:

            By setting the clipNode above the geometry nodes of the plot items
            we only need on clip node for all plot items. However we exclude all
            nodes from scene graph batching. It might be better that each plot
            item has its own clip node. But we could share their clipping vertexes.
            TODO ...
         */

        itemsNode = QskSkinlet::updateBoxClipNode( this,
            itemsNode, subControlRect( Canvas ), Canvas );
    }

    for ( auto& itemData : m_data->itemData )
    {
        if ( danglingNodes || itemData.node == nullptr )
        {
            itemData.node = new QSGTransformNode();
            itemsNode->appendChildNode( itemData.node );
        }

        QMatrix4x4 matrix;

        if ( itemData.plotItem->coordinateType() == QskPlotItem::PlotCoordinates )
            matrix = itemData.plotItem->transformation();

        if ( matrix != itemData.node->matrix() )
            itemData.node->setMatrix( matrix );

        if ( itemData.plotItem->isDirty() )
            itemData.plotItem->updateNode( itemData.node );
    }
}

#include "moc_QskPlotView.cpp"
