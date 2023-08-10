/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskListViewSkinlet.h"
#include "QskListView.h"

#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskBoxHints.h"
#include "QskSGNode.h"
#include "QskSkinStateChanger.h"
#include "QskQuick.h"

#include <qmath.h>
#include <qsgnode.h>
#include <qtransform.h>

namespace
{
    class ForegroundNode : public QSGNode
    {
      public:
        void invalidate()
        {
            removeAllChildNodes();
            m_columnCount = m_oldRowMin = m_oldRowMax = -1;
        }

        void rearrangeNodes( int rowMin, int rowMax, int columnCount )
        {
            const bool doReorder = ( columnCount == m_columnCount )
                && ( rowMin <= m_oldRowMax ) && ( rowMax >= m_oldRowMin );

            if ( doReorder )
            {
                /*
                    We have nodes that will be at a different position in the
                    list of child nodes - however their content might be unchanged.

                    This is a common situation, when resizing or scrolling the list.

                    To avoid that the text/graphic nodes have to be updated we
                    try to rearrange the nodes, so that they are in increasing
                    row order for further processing.

                    To avoid delete/new calls we append the leading and prepend
                    the trailing nodes, so that they might be used later.
                 */

                if ( rowMin >= m_oldRowMin )
                {
                    for ( int row = m_oldRowMin; row < rowMin; row++ )
                    {
                        for ( int col = 0; col < columnCount; col++ )
                        {
                            auto childNode = firstChild();
                            removeChildNode( childNode );
                            appendChildNode( childNode );
                        }
                    }
                }
                else
                {
                    for ( int row = rowMin; row < m_oldRowMin; row++ )
                    {
                        for ( int col = 0; col < columnCount; col++ )
                        {
                            auto childNode = lastChild();
                            removeChildNode( childNode );
                            prependChildNode( childNode );
                        }
                    }
                }
            }

            m_oldRowMin = rowMin;
            m_oldRowMax = rowMax;
            m_columnCount = columnCount;
        }

      private:
        /*
            When scrolling the majority of the child nodes are simply translated
            while only few rows appear/disappear. To implement this in an efficient
            way we store how the nodes were related to the rows in the previous update,
         */

        int m_oldRowMin = -1;
        int m_oldRowMax = -1;
        int m_columnCount = -1;
    };

    class ListViewNode final : public QSGTransformNode
    {
      public:
        inline ListViewNode()
        {
            m_backgroundNode.setFlag( QSGNode::OwnedByParent, false );
            appendChildNode( &m_backgroundNode );

            m_foregroundNode.setFlag( QSGNode::OwnedByParent, false );
            appendChildNode( &m_foregroundNode );
        }

        void initialize( const QskListView* listView )
        {
            const auto scrollPos = listView->scrollPos();
            setMatrix( QTransform::fromTranslate( -scrollPos.x(), -scrollPos.y() ) );

            m_clipRect = listView->viewContentsRect();
            m_rowHeight = listView->rowHeight();

            m_rowMin = qFloor( scrollPos.y() / m_rowHeight );

            const auto rowMax = ( scrollPos.y() + m_clipRect.height() ) / m_rowHeight;
            m_rowMax = qFloor( rowMax - 10e-6 );

            if ( m_rowMax >= listView->rowCount() )
                m_rowMax = listView->rowCount() - 1;
        }

        QRectF clipRect() const { return m_clipRect; }

        int rowMin() const { return m_rowMin; }
        int rowMax() const { return m_rowMax; }
        int rowCount() const { return m_rowMax - m_rowMin + 1; }

        int rowHeight() const { return m_rowHeight; }

        QSGNode* backgroundNode() { return &m_backgroundNode; }
        ForegroundNode* foregroundNode() { return &m_foregroundNode; }

      private:
        // caching some calculations to speed things up

        QRectF m_clipRect;
        qreal m_rowHeight;

        int m_rowMin, m_rowMax;

        QSGNode m_backgroundNode;
        ForegroundNode m_foregroundNode;
    };
}

static inline ListViewNode* qskListViewNode( const QskListView* listView )
{
    if ( auto node = const_cast< QSGNode* >( qskPaintNode( listView ) ) )
    {
        using namespace QskSGNode;

        node = findChildNode( node, QskScrollViewSkinlet::ContentsRootRole );
        if ( node )
        {
            node = node->firstChild();
            if ( node )
            {
                Q_ASSERT( node->type() == QSGNode::TransformNodeType );
                return static_cast< ListViewNode* >( node );
            }
        }
    }

    return nullptr;
}

static inline ListViewNode* qskListViewNode( const QskSkinnable* skinnable )
{
    return qskListViewNode( static_cast< const QskListView* >( skinnable ) );
}

QskListViewSkinlet::QskListViewSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskListViewSkinlet::~QskListViewSkinlet() = default;

QSGNode* QskListViewSkinlet::updateContentsNode(
    const QskScrollView* scrollView, QSGNode* node ) const
{
    const auto listView = static_cast< const QskListView* >( scrollView );

    auto listViewNode = QskSGNode::ensureNode< ListViewNode >( node );
    listViewNode->initialize( listView );

    updateBackgroundNodes( listView, listViewNode->backgroundNode() );
    updateForegroundNodes( listView, listViewNode->foregroundNode() );

    return listViewNode;
}

void QskListViewSkinlet::updateBackgroundNodes(
    const QskListView* listView, QSGNode* backgroundNode ) const
{
    using Q = QskListView;

    auto listViewNode = static_cast< const ListViewNode* >( backgroundNode->parent() );

    auto rowNode = backgroundNode->firstChild();

    for ( int row = listViewNode->rowMin(); row <= listViewNode->rowMax(); row++ )
    {
        QskSkinStateChanger stateChanger( listView );
        stateChanger.setStates( sampleStates( listView, Q::Cell, row ), row );

        const auto rect = sampleRect( listView, listView->contentsRect(), Q::Cell, row );

        auto newNode = updateBoxNode( listView, rowNode, rect, Q::Cell );
        if ( newNode )
        {
            if ( newNode->parent() != backgroundNode )
                backgroundNode->appendChildNode( newNode );
            else
                rowNode = newNode->nextSibling();
        }
    }

    QskSGNode::removeAllChildNodesFrom( backgroundNode, rowNode );
}

void QskListViewSkinlet::updateForegroundNodes(
    const QskListView* listView, QSGNode* parentNode ) const
{
    auto foregroundNode = static_cast< ForegroundNode* >( parentNode );

    if ( listView->rowCount() <= 0 || listView->columnCount() <= 0 )
    {
        foregroundNode->invalidate();
        return;
    }

    auto listViewNode = static_cast< const ListViewNode* >( parentNode->parent() );

    const auto clipRect = listViewNode->clipRect();

    const int rowMin = listViewNode->rowMin();
    const int rowMax = listViewNode->rowMax();

    foregroundNode->rearrangeNodes( rowMin, rowMax, listView->columnCount() );

#if 1
    // should be optimized for visible columns only
    const int colMin = 0;
    const int colMax = listView->columnCount() - 1;
#endif

    const auto margins = listView->paddingHint( QskListView::Cell );

    updateVisibleForegroundNodes(
        listView, foregroundNode, rowMin, rowMax, margins );

    // finally putting the nodes into their position
    auto node = foregroundNode->firstChild();

    const auto rowHeight = listView->rowHeight();
    auto y = clipRect.top() + rowMin * rowHeight;

    for ( int row = rowMin; row <= rowMax; row++ )
    {
        qreal x = clipRect.left();

        for ( int col = colMin; col <= colMax; col++ )
        {
            Q_ASSERT( node->type() == QSGNode::TransformNodeType );
            auto transformNode = static_cast< QSGTransformNode* >( node );

            transformNode->setMatrix(
                QTransform::fromTranslate( x + margins.left(), y + margins.top() ) );

            node = node->nextSibling();
            x += listView->columnWidth( col );
        }

        y += rowHeight;
    }
}

void QskListViewSkinlet::updateVisibleForegroundNodes(
    const QskListView* listView, QSGNode* parentNode,
    int rowMin, int rowMax, const QMarginsF& margins ) const
{
    auto node = parentNode->firstChild();

    for ( int row = rowMin; row <= rowMax; row++ )
    {
        const auto h = listView->rowHeight() - ( margins.top() + margins.bottom() );

        for ( int col = 0; col < listView->columnCount(); col++ )
        {
            const auto w = listView->columnWidth( col ) - ( margins.left() + margins.right() );

            node = updateForegroundNode( listView,
                parentNode, static_cast< QSGTransformNode* >( node ),
                row, col, QSizeF( w, h ) );

            node = node->nextSibling();
        }
    }

    QskSGNode::removeAllChildNodesFrom( parentNode, node );
}

QSGTransformNode* QskListViewSkinlet::updateForegroundNode(
    const QskListView* listView, QSGNode* parentNode, QSGTransformNode* cellNode,
    int row, int col, const QSizeF& size ) const
{
    const QRectF cellRect( 0.0, 0.0, size.width(), size.height() );

    /*
        Text nodes already have a transform root node - to avoid inserting extra
        transform nodes, the code below becomes a bit more complicated.
     */
    QSGTransformNode* newCellNode = nullptr;

    if ( cellNode && ( cellNode->type() == QSGNode::TransformNodeType ) )
    {
        QSGNode* oldNode = cellNode;

        auto newNode = updateCellNode( listView, oldNode, cellRect, row, col );
        if ( newNode )
        {
            if ( newNode->type() == QSGNode::TransformNodeType )
            {
                newCellNode = static_cast< QSGTransformNode* >( newNode );
            }
            else
            {
                newCellNode = new QSGTransformNode();
                newCellNode->appendChildNode( newNode );
            }
        }
    }
    else
    {
        QSGNode* oldNode = cellNode ? cellNode->firstChild() : nullptr;
        auto newNode = updateCellNode( listView, oldNode, cellRect, row, col );

        if ( newNode )
        {
            if ( newNode->type() == QSGNode::TransformNodeType )
            {
                newCellNode = static_cast< QSGTransformNode* >( newNode );
            }
            else
            {
                if ( cellNode == nullptr )
                {
                    newCellNode = new QSGTransformNode();
                    newCellNode->appendChildNode( newNode );
                }
                else
                {
                    if ( newNode != oldNode )
                    {
                        delete cellNode->firstChild();
                        cellNode->appendChildNode( newNode );

                        newCellNode = cellNode;
                    }
                }
            }
        }
    }

    if ( newCellNode == nullptr )
        newCellNode = new QSGTransformNode();

    if ( cellNode != newCellNode )
    {
        if ( cellNode )
        {
            parentNode->insertChildNodeAfter( newCellNode, cellNode );
            delete cellNode;
        }
        else
        {
            parentNode->appendChildNode( newCellNode );
        }
    }

    return newCellNode;
}

QSGNode* QskListViewSkinlet::updateCellNode( const QskListView* listView,
    QSGNode* contentNode, const QRectF& rect, int row, int col ) const
{
    using Q = QskListView;
    using namespace QskSGNode;

    QskSkinStateChanger stateChanger( listView );
    stateChanger.setStates( sampleStates( listView, Q::Cell, row ), row );

    QSGNode* newNode = nullptr;

#if 1
    /*
        Alignments, text options etc are user definable attributes and need
        to be adjustable - at least individually for each column - from the
        public API of QskListView TODO ...
     */
#endif
    const auto alignment = listView->alignmentHint(
        Q::Cell, Qt::AlignVCenter | Qt::AlignLeft );

    const auto value = listView->valueAt( row, col );

    if ( value.canConvert< QskGraphic >() )
    {
        if ( nodeRole( contentNode ) == GraphicRole )
            newNode = contentNode;

        const auto colorFilter = listView->effectiveGraphicFilter( Q::Graphic );

        newNode = updateGraphicNode( listView, newNode,
            value.value< QskGraphic >(), colorFilter, rect, alignment );

        if ( newNode )
            setNodeRole( newNode, GraphicRole );
    }
    else if ( value.canConvert< QString >() )
    {
        if ( nodeRole( contentNode ) == TextRole )
            newNode = contentNode;

        newNode = updateTextNode( listView, newNode, rect, alignment,
            value.toString(), Q::Text );

        if ( newNode )
            setNodeRole( newNode, TextRole );
    }
    else
    {
        qWarning() << "QskListViewSkinlet: got unsupported QVariant type" << value.typeName();
    }

    return newNode;
}

QSizeF QskListViewSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    const auto listView = static_cast< const QskListView* >( skinnable );

    qreal w = -1.0; // shouldn't we return something ???

    if ( which != Qt::MaximumSize )
    {
        if ( listView->preferredWidthFromColumns() )
        {
            w = listView->scrollableSize().width();
            w += listView->metric( QskScrollView::VerticalScrollBar | QskAspect::Size );
        }
    }

    return QSizeF( w, -1.0 );
}

QskAspect::States QskListViewSkinlet::sampleStates( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskListView;

    if ( subControl == Q::Cell || subControl == Q::Text || subControl == Q::Graphic )
    {
        const auto listView = static_cast< const QskListView* >( skinnable );
        return listView->rowStates( index );
    }

    return Inherited::sampleStates( skinnable, subControl, index );
}

QRectF QskListViewSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskListView;

    const auto listView = static_cast< const QskListView* >( skinnable );

    if ( subControl == Q::Cell )
    {
        auto node = qskListViewNode( listView );
        const auto clipRect = node ? node->clipRect() : listView->viewContentsRect();

        const auto w = clipRect.width();
        const auto h = listView->rowHeight();
        const auto x = clipRect.left() + listView->scrollPos().x();
        const auto y = clipRect.top() + index * h;

        return QRectF( x, y, w, h );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

#include "moc_QskListViewSkinlet.cpp"
