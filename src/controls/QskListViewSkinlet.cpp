/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskListViewSkinlet.h"
#include "QskListView.h"

#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskSGNode.h"

#include <qmath.h>
#include <qsgnode.h>
#include <qsgsimplerectnode.h>
#include <qtransform.h>

class QskListViewNode final : public QSGTransformNode
{
  public:
    inline QskListViewNode( int columnCount )
        : m_columnCount( columnCount )
        , m_rowMin( -1 )
        , m_rowMax( -1 )
    {
        m_backgroundNode.setFlag( QSGNode::OwnedByParent, false );
        appendChildNode( &m_backgroundNode );

        m_foregroundNode.setFlag( QSGNode::OwnedByParent, false );
        appendChildNode( &m_foregroundNode );
    }

    QSGNode* backgroundNode()
    {
        return &m_backgroundNode;
    }

    QSGNode* foregroundNode()
    {
        return &m_foregroundNode;
    }

    inline void resetRows( int rowMin, int rowMax )
    {
        m_rowMin = rowMin;
        m_rowMax = rowMax;
    }

    inline int rowMin() const
    {
        return m_rowMin;
    }

    inline int rowMax() const
    {
        return m_rowMax;
    }

    inline bool intersects( int rowMin, int rowMax ) const
    {
        return ( rowMin <= m_rowMax ) && ( rowMax >= m_rowMin );
    }

    inline int nodeCount() const
    {
        return ( m_rowMin >= 0 ) ? ( m_rowMax - m_rowMin + 1 ) * m_columnCount : 0;
    }

    inline int columnCount() const
    {
        return m_columnCount;
    }

    inline void invalidate()
    {
        m_rowMin = m_rowMax = -1;
    }

  private:
    int m_columnCount;
    int m_rowMin;
    int m_rowMax;

    QSGNode m_backgroundNode;
    QSGNode m_foregroundNode;
};

QskListViewSkinlet::QskListViewSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskListViewSkinlet::~QskListViewSkinlet() = default;

QSGNode* QskListViewSkinlet::updateContentsNode(
    const QskScrollView* scrollView, QSGNode* node ) const
{
    const auto listView = static_cast< const QskListView* >( scrollView );

    auto* listViewNode = static_cast< QskListViewNode* >( node );
    if ( listViewNode == nullptr )
        listViewNode = new QskListViewNode( listView->columnCount() );

    QTransform transform;
    transform.translate( -listView->scrollPos().x(), -listView->scrollPos().y() );
    listViewNode->setMatrix( transform );

    updateBackgroundNodes( listView, listViewNode );
    updateForegroundNodes( listView, listViewNode );

    return listViewNode;
}

void QskListViewSkinlet::updateBackgroundNodes(
    const QskListView* listView, QskListViewNode* listViewNode ) const
{
    QSGNode* backgroundNode = listViewNode->backgroundNode();

    const qreal cellHeight = listView->rowHeight();
    const QRectF viewRect = listView->viewContentsRect();

    const QPointF scrolledPos = listView->scrollPos();
    const int rowMin = qFloor( scrolledPos.y() / cellHeight );

    int rowMax = qCeil( ( scrolledPos.y() + viewRect.height() ) / cellHeight );
    if ( rowMax >= listView->rowCount() )
        rowMax = listView->rowCount() - 1;

    const int rowSelected = listView->selectedRow();
    const double x0 = viewRect.left() + scrolledPos.x();
    const double y0 = viewRect.top();

    auto* rowNode = static_cast< QSGSimpleRectNode* >( backgroundNode->firstChild() );

    if ( listView->alternatingRowColors() )
    {
#if 1
        /*
            Cell might be better for regular cells, while ( Cell | AlternateColor )
            could be used for the alternate color TODO ...
         */
#endif
        const auto color = listView->color( QskListView::Cell );

        for ( int row = rowMin; row <= rowMax; row++ )
        {
            if ( row % 2 )
            {
                if ( rowNode == nullptr )
                {
                    rowNode = new QSGSimpleRectNode();
                    backgroundNode->appendChildNode( rowNode );
                }

                rowNode->setRect( x0, y0 + row * cellHeight, viewRect.width(), cellHeight );
                rowNode->setColor( color );

                rowNode = static_cast< QSGSimpleRectNode* >( rowNode->nextSibling() );
            }
        }
    }

    if ( rowSelected >= rowMin && rowSelected <= rowMax )
    {
        const QColor color = listView->color( QskListView::CellSelected );

        if ( rowNode == nullptr )
        {
            rowNode = new QSGSimpleRectNode();
            backgroundNode->appendChildNode( rowNode );
        }

        rowNode->setRect( x0, y0 + rowSelected * cellHeight, viewRect.width(), cellHeight );
        rowNode->setColor( color );

        rowNode = static_cast< QSGSimpleRectNode* >( rowNode->nextSibling() );
    }

    QSGNode* nextNode = rowNode;
    while ( nextNode != nullptr )
    {
        QSGNode* tmpNode = nextNode;
        nextNode = nextNode->nextSibling();
        delete tmpNode;
    }
}

void QskListViewSkinlet::updateForegroundNodes(
    const QskListView* listView, QskListViewNode* listViewNode ) const
{
    auto parentNode = listViewNode->foregroundNode();

    if ( listView->rowCount() <= 0 || listView->columnCount() <= 0 )
    {
        parentNode->removeAllChildNodes();
        listViewNode->invalidate();
        return;
    }

    const auto margins = listView->paddingHint( QskListView::Cell );

    const auto cr = listView->viewContentsRect();
    const auto scrolledPos = listView->scrollPos();

    const int rowMin = qFloor( scrolledPos.y() / listView->rowHeight() );

    int rowMax = rowMin + qCeil( cr.height() / listView->rowHeight() );
    if ( rowMax >= listView->rowCount() )
        rowMax = listView->rowCount() - 1;

#if 1
    // should be optimized for visible columns only
    const int colMin = 0;
    const int colMax = listView->columnCount() - 1;
#endif

    bool forwards = true;

    if ( listViewNode->intersects( rowMin, rowMax ) )
    {
        /*
            We try to avoid reallcations when scrolling, by reusing
            the nodes of the cells leaving the viewport for those becoming visible.
         */

        forwards = ( rowMin >= listViewNode->rowMin() );

        if ( forwards )
        {
            // usually scrolling down
            for ( int row = listViewNode->rowMin(); row < rowMin; row++ )
            {
                for ( int col = 0; col < listView->columnCount(); col++ )
                {
                    QSGNode* childNode = parentNode->firstChild();
                    parentNode->removeChildNode( childNode );
                    parentNode->appendChildNode( childNode );
                }
            }
        }
        else
        {
            // usually scrolling up
            for ( int row = rowMax; row < listViewNode->rowMax(); row++ )
            {
                for ( int col = 0; col < listView->columnCount(); col++ )
                {
                    QSGNode* childNode = parentNode->lastChild();
                    parentNode->removeChildNode( childNode );
                    parentNode->prependChildNode( childNode );
                }
            }
        }
    }

    updateVisibleForegroundNodes( listView, listViewNode,
        rowMin, rowMax, colMin, colMax, margins, forwards );

    // finally putting the nodes into their position
    auto node = parentNode->firstChild();

    const qreal rowHeight = listView->rowHeight();
    qreal y = cr.top() + rowMin * rowHeight;

    for ( int row = rowMin; row <= rowMax; row++ )
    {
        qreal x = cr.left();

        for ( int col = colMin; col <= colMax; col++ )
        {
            Q_ASSERT( node->type() == QSGNode::TransformNodeType );
            auto transformNode = static_cast< QSGTransformNode* >( node );

            QTransform transform;
            transform.translate( x + margins.left(), y + margins.top() );

            transformNode->setMatrix( transform );

            node = node->nextSibling();
            x += listView->columnWidth( col );
        }

        y += rowHeight;
    }

    listViewNode->resetRows( rowMin, rowMax );
}

void QskListViewSkinlet::updateVisibleForegroundNodes(
    const QskListView* listView, QskListViewNode* listViewNode,
    int rowMin, int rowMax, int colMin, int colMax, const QMarginsF& margins,
    bool forward ) const
{
    auto parentNode = listViewNode->foregroundNode();

    const int rowCount = rowMax - rowMin + 1;
    const int colCount = colMax - colMin + 1;
    const int obsoleteNodesCount = listViewNode->nodeCount() - rowCount * colCount;

    if ( forward )
    {
        for ( int i = 0; i < obsoleteNodesCount; i++ )
            delete parentNode->lastChild();

        auto node = parentNode->firstChild();

        for ( int row = rowMin; row <= rowMax; row++ )
        {
            const qreal h = listView->rowHeight() - ( margins.top() + margins.bottom() );

            for ( int col = 0; col < listView->columnCount(); col++ )
            {
                const qreal w = listView->columnWidth( col ) - ( margins.left() + margins.right() );

                node = updateForegroundNode( listView,
                    parentNode, static_cast< QSGTransformNode* >( node ),
                    row, col, QSizeF( w, h ), forward );

                node = node->nextSibling();
            }
        }
    }
    else
    {
        for ( int i = 0; i < obsoleteNodesCount; i++ )
            delete parentNode->firstChild();

        auto* node = parentNode->lastChild();

        for ( int row = rowMax; row >= rowMin; row-- )
        {
            const qreal h = listView->rowHeight() - ( margins.top() + margins.bottom() );

            for ( int col = listView->columnCount() - 1; col >= 0; col-- )
            {
                const qreal w = listView->columnWidth( col ) - ( margins.left() + margins.right() );

                node = updateForegroundNode( listView,
                    parentNode, static_cast< QSGTransformNode* >( node ),
                    row, col, QSizeF( w, h ), forward );

                node = node->previousSibling();
            }
        }
    }
}

QSGTransformNode* QskListViewSkinlet::updateForegroundNode(
    const QskListView* listView, QSGNode* parentNode, QSGTransformNode* cellNode,
    int row, int col, const QSizeF& size, bool forward ) const
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
            if ( forward )
                parentNode->appendChildNode( newCellNode );
            else
                parentNode->prependChildNode( newCellNode );
        }
    }

    return newCellNode;
}

QSGNode* QskListViewSkinlet::updateCellNode( const QskListView* listView,
    QSGNode* contentNode, const QRectF& rect, int row, int col ) const
{
    using namespace QskSGNode;

    QSGNode* newNode = nullptr;

#if 1
    /*
        Alignments, text options etc are user definable attributes and need
        to be adjustable - at least individually for each column - from the
        public API of QskListView TODO ...
     */
#endif
    const auto alignment = listView->alignmentHint(
        QskListView::Cell, Qt::AlignVCenter | Qt::AlignLeft );

    const auto value = listView->valueAt( row, col );

    if ( value.canConvert< QskGraphic >() )
    {
        if ( nodeRole( contentNode ) == GraphicRole )
            newNode = contentNode;

        const auto colorFilter = listView->graphicFilterAt( row, col );

        newNode = updateGraphicNode( listView, newNode,
            value.value< QskGraphic >(), colorFilter, rect, alignment );

        if ( newNode )
            setNodeRole( newNode, GraphicRole );
    }
    else if ( value.canConvert< QString >() )
    {
        if ( nodeRole( contentNode ) == TextRole )
            newNode = contentNode;

        auto subControl = listView->textSubControlAt( row, col );

        newNode = updateTextNode( listView, newNode, rect, alignment,
            value.toString(), listView->textOptions(), subControl );

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

#include "moc_QskListViewSkinlet.cpp"
