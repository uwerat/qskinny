/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScaleRenderer.h"
#include "QskScaleTickmarks.h"
#include "QskSkinlet.h"
#include "QskSGNode.h"
#include "QskTickmarksNode.h"
#include "QskTextOptions.h"
#include "QskTextColors.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskIntervalF.h"
#include "QskFunctions.h"

#include <qstring.h>
#include <qfontmetrics.h>

static QSGNode* qskRemoveTraillingNodes( QSGNode* node, QSGNode* childNode )
{
    QskSGNode::removeAllChildNodesFrom( node, childNode );
    return nullptr;
}

static inline void qskInsertRemoveChild( QSGNode* parentNode,
    QSGNode* oldNode, QSGNode* newNode, bool append )
{
    if ( newNode == oldNode )
        return;

    if ( oldNode )
    {
        parentNode->removeChildNode( oldNode );
        if ( oldNode->flags() & QSGNode::OwnedByParent )
            delete oldNode;
    }

    if ( newNode )
    {
        if ( append )
            parentNode->appendChildNode( newNode );
        else
            parentNode->prependChildNode( newNode );
    }
}

class QskScaleRenderer::PrivateData
{
  public:
    QskIntervalF boundaries;
    QskScaleTickmarks tickmarks;

    QColor tickColor = Qt::black;
    qreal tickWidth = 1.0;

    QFont font;
    QskTextColors textColors;

    QskColorFilter colorFilter;

    Qt::Orientation orientation = Qt::Horizontal;
};

QskScaleRenderer::QskScaleRenderer()
    : m_data( new PrivateData() )
{
}

QskScaleRenderer::~QskScaleRenderer()
{
}

void QskScaleRenderer::setOrientation( Qt::Orientation orientation )
{
    m_data->orientation = orientation;
}

void QskScaleRenderer::setBoundaries( const QskIntervalF& boundaries )
{
    m_data->boundaries = boundaries;
}

void QskScaleRenderer::setTickmarks( const QskScaleTickmarks& tickmarks )
{
    m_data->tickmarks = tickmarks;
}

void QskScaleRenderer::setTickColor( const QColor& color )
{
    m_data->tickColor = color;
}

void QskScaleRenderer::setTickWidth( qreal width )
{
    m_data->tickWidth = width;
}

void QskScaleRenderer::setFont( const QFont& font )
{
    m_data->font = font;
}

void QskScaleRenderer::setTextColors( const QskTextColors& textColors )
{
    m_data->textColors = textColors;
}

void QskScaleRenderer::setColorFilter( const QskColorFilter& colorFilter )
{
    m_data->colorFilter = colorFilter;
}

QSGNode* QskScaleRenderer::updateScaleNode(
    const QskSkinnable* skinnable, const QRectF& tickmarksRect,
    const QRectF& labelsRect, QSGNode* node )
{
    enum Role
    {
        Ticks = 1,
        Labels = 2
    };

    if ( node == nullptr )
        node = new QSGNode();

    {
        QSGNode* oldNode = QskSGNode::findChildNode( node, Ticks );
        QSGNode* newNode = nullptr;

        if ( !tickmarksRect.isEmpty() )
        {
            newNode = updateTicksNode( skinnable, tickmarksRect, oldNode );
            if ( newNode )
                QskSGNode::setNodeRole( newNode, Ticks );
        }

        qskInsertRemoveChild( node, oldNode, newNode, false );
    }

    {
        QSGNode* oldNode = QskSGNode::findChildNode( node, Labels );
        QSGNode* newNode = nullptr;

        if ( !labelsRect.isEmpty() )
        {
            newNode = updateLabelsNode( skinnable, tickmarksRect, labelsRect, oldNode );
            if ( newNode )
                QskSGNode::setNodeRole( newNode, Labels );
        }

        qskInsertRemoveChild( node, oldNode, newNode, true );
    }

    return node;
}

QSGNode* QskScaleRenderer::updateTicksNode(
    const QskSkinnable*, const QRectF& rect, QSGNode* node ) const
{
    if ( rect.isEmpty() )
        return nullptr;

    auto ticksNode = static_cast< QskTickmarksNode* >( node );

    if( ticksNode == nullptr )
        ticksNode = new QskTickmarksNode;

    ticksNode->update( m_data->tickColor, rect, m_data->boundaries,
        m_data->tickmarks, m_data->tickWidth, m_data->orientation );

    return ticksNode;
}

QSGNode* QskScaleRenderer::updateLabelsNode(
    const QskSkinnable* skinnable, const QRectF& tickmarksRect,
    const QRectF& labelsRect, QSGNode* node ) const
{
    if ( labelsRect.isEmpty() || tickmarksRect.isEmpty() )
        return nullptr;

    const auto ticks = m_data->tickmarks.majorTicks();
    if ( ticks.isEmpty() )
        return nullptr;

    if( node == nullptr )
        node = new QSGNode;

    const QFontMetricsF fm( m_data->font );

    const qreal length = ( m_data->orientation == Qt::Horizontal )
        ? tickmarksRect.width() : tickmarksRect.height();
    const qreal ratio = length / m_data->boundaries.width();

    auto nextNode = node->firstChild();

    QRectF labelRect;

    for ( auto tick : ticks )
    {
        enum LabelNodeRole
        {
            TextNode = 1,
            GraphicNode = 2
        };

        const auto label = labelAt( tick );
        if ( label.isNull() )
            continue;

        const qreal tickPos = ratio * ( tick - m_data->boundaries.lowerBound() );

        if ( label.canConvert< QString >() )
        {
            auto text = label.toString();
            if ( text.isEmpty() )
                continue;

            QRectF r;
            Qt::Alignment alignment;

            if( m_data->orientation == Qt::Horizontal )
            {
                const auto w = qskHorizontalAdvance( fm, text );

                auto pos = tickmarksRect.x() + tickPos - 0.5 * w;
                pos = qBound( labelsRect.left(), pos, labelsRect.right() - w );

                r = QRectF( pos, labelsRect.y(), w, labelsRect.height() );

                alignment = Qt::AlignLeft;
            }
            else
            {
                const auto h = fm.height();

                auto pos = tickmarksRect.bottom() - ( tickPos + 0.5 * h );

                /*
                    when clipping the label we can expand the clip rectangle
                    by the ascent/descent margins, as nothing gets painted there
                    anyway.
                 */
                const qreal min = labelsRect.top() - ( h - fm.ascent() );
                const qreal max = labelsRect.bottom() + fm.descent();
                pos = qBound( min, pos, max );

                r = QRectF( labelsRect.x(), pos, labelsRect.width(), h );

                alignment = Qt::AlignRight;
            }

            if ( nextNode && QskSGNode::nodeRole( nextNode ) != TextNode )
            {
                nextNode = qskRemoveTraillingNodes( node, nextNode );
            }

            if ( !labelRect.isEmpty() && labelRect.intersects( r ) )
            {
                if ( tick != ticks.last() )
                {
                    text = QString();
                }
                else
                {
                    if ( auto obsoleteNode = nextNode
                        ? nextNode->previousSibling() : node->lastChild() )
                    {
                        node->removeChildNode( obsoleteNode );
                        if ( obsoleteNode->flags() & QSGNode::OwnedByParent )
                            delete obsoleteNode;
                    }

                    labelRect = r;
                }
            }
            else
            {
                labelRect = r;
            }

            nextNode = QskSkinlet::updateTextNode( skinnable, nextNode,
                r, alignment, text, m_data->font, QskTextOptions(),
                m_data->textColors, Qsk::Normal );

            if ( nextNode )
            {
                if ( nextNode->parent() != node )
                {
                    QskSGNode::setNodeRole( nextNode, TextNode );
                    node->appendChildNode( nextNode );
                }

                nextNode = nextNode->nextSibling();
            }
        }
        else if ( label.canConvert< QskGraphic >() )
        {
            const auto graphic = label.value< QskGraphic >();
            if ( graphic.isNull() )
                continue;

            const auto h = fm.height();
            const auto w = graphic.widthForHeight( h );

            Qt::Alignment alignment;

            if( m_data->orientation == Qt::Horizontal )
            {
                auto pos = tickmarksRect.x() + tickPos - 0.5 * w;
                pos = qBound( labelsRect.left(), pos, labelsRect.right() - w );

                labelRect = QRectF( pos, labelsRect.y(), w, h );
                alignment = Qt::AlignHCenter | Qt::AlignBottom;
            }
            else
            {
                auto pos = tickmarksRect.bottom() - ( tickPos + 0.5 * h );
                pos = qBound( labelsRect.top(), pos, labelsRect.bottom() - h );

                labelRect = QRectF( labelsRect.right() - w, pos, w, h );
                alignment = Qt::AlignRight | Qt::AlignVCenter;
            }

            if ( nextNode && QskSGNode::nodeRole( nextNode ) != GraphicNode )
            {
                nextNode = qskRemoveTraillingNodes( node, nextNode );
            }

            nextNode = QskSkinlet::updateGraphicNode(
                skinnable, nextNode, graphic, m_data->colorFilter, labelRect, alignment );

            if ( nextNode )
            {
                if ( nextNode->parent() != node )
                {
                    QskSGNode::setNodeRole( nextNode, GraphicNode );
                    node->appendChildNode( nextNode );
                }

                nextNode = nextNode->nextSibling();
            }
        }
    }

    qskRemoveTraillingNodes( node, nextNode );

    return node;
}

QVariant QskScaleRenderer::labelAt( qreal pos ) const
{
    return QString::number( pos, 'g' );
}

QSizeF QskScaleRenderer::boundingLabelSize() const
{
    const auto ticks = m_data->tickmarks.majorTicks();
    if ( ticks.isEmpty() )
        return QSizeF( 0.0, 0.0 );

    const QFontMetricsF fm( m_data->font );

    qreal maxWidth = 0.0;
    const qreal h = fm.height();

    for ( auto tick : ticks )
    {
        qreal w = 0.0;

        const auto label = labelAt( tick );
        if ( label.isNull() )
            continue;

        if ( label.canConvert< QString >() )
        {
            w = qskHorizontalAdvance( fm, label.toString() );
        }
        else if ( label.canConvert< QskGraphic >() )
        {
            const auto graphic = label.value< QskGraphic >();
            if ( !graphic.isNull() )
            {
                w = graphic.widthForHeight( h );
            }
        }

        maxWidth = qMax( w, maxWidth );
    }

    return QSizeF( maxWidth, h );
}
