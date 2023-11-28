/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskScaleRenderer.h"
#include "QskTickmarks.h"
#include "QskSkinlet.h"
#include "QskSGNode.h"
#include "QskAxisScaleNode.h"
#include "QskTextOptions.h"
#include "QskTextColors.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskIntervalF.h"
#include "QskFunctions.h"

#include <qstring.h>
#include <qfontmetrics.h>
#include <qquickwindow.h>

namespace
{
    class ScaleMap
    {
      public:
        inline ScaleMap( bool isHorizontal, const QTransform& transform )
            : t( isHorizontal ? transform.dx() : transform.dy() )
            , f( isHorizontal ? transform.m11() : transform.m22() )
        {
        }

        inline qreal map( qreal v ) const { return t + f * v; };

      private:
        const qreal t;
        const qreal f;
    };
}

static inline bool qskIsHorizontal( Qt::Edge edge )
{
    return edge & ( Qt::TopEdge | Qt::BottomEdge );
}

static QSGNode* qskRemoveTraillingNodes( QSGNode* node, QSGNode* childNode )
{
    QskSGNode::removeAllChildNodesFrom( node, childNode );
    return nullptr;
}

static inline QTransform qskScaleTransform( Qt::Edge edge,
    const QskIntervalF& boundaries, const QskIntervalF& range )
{
    using T = QTransform;

    if ( qskIsHorizontal( edge ) )
    {
        auto transform = T::fromTranslate( -boundaries.lowerBound(), 0.0 );
        transform *= T::fromScale( range.length() / boundaries.length(), 1.0 );
        transform *= T::fromTranslate( range.lowerBound(), 0.0 );

        return transform;
    }
    else
    {
        auto transform = T::fromTranslate( 0.0, -boundaries.lowerBound() );
        transform *= T::fromScale( 1.0, -range.length() / boundaries.length() );
        transform *= T::fromTranslate( 0.0, range.upperBound() );

        return transform;
    }
}

static inline quint8 qskLabelNodeRole( const QVariant& label )
{
    if ( !label.isNull() )
    {
        if ( label.canConvert< QString >() )
            return 1;

        if ( label.canConvert< QskGraphic >() )
            return 2;
    }

    return QskSGNode::NoRole;
}

class QskScaleRenderer::PrivateData
{
  public:

    // Coordinates related to the scales
    QskIntervalF boundaries;
    QskTickmarks tickmarks;

    /*
        Item cooordinates. In case of an horizontal scale
        position is an y coordinate, while range corresponds to x coordinates
        ( vertical: v.v )
     */
    qreal position = 0.0;
    QskIntervalF range;

#if 1
    QColor tickColor = Qt::black; // rgb value ???
#endif

    qreal tickWidth = 1.0;
    qreal tickLength = 10.0;
    qreal spacing = 5.0;

    QFont font;
    QskTextColors textColors;

    QskColorFilter colorFilter;

    Qt::Edge edge = Qt::BottomEdge;
    QskScaleRenderer::Flags flags = ClampedLabels;
};

QskScaleRenderer::QskScaleRenderer()
    : m_data( new PrivateData() )
{
}

QskScaleRenderer::~QskScaleRenderer()
{
}

void QskScaleRenderer::setEdge( Qt::Edge edge )
{
    m_data->edge = edge;
}

Qt::Edge QskScaleRenderer::edge() const
{
    return m_data->edge;
}

void QskScaleRenderer::setFlag( Flag flag, bool on )
{
    if ( on )
        m_data->flags |= flag;
    else
        m_data->flags &= ~flag;
}

void QskScaleRenderer::setFlags( Flags flags )
{
    m_data->flags = flags;
}

QskScaleRenderer::Flags QskScaleRenderer::flags() const
{
    return m_data->flags;
}

void QskScaleRenderer::setBoundaries( qreal lowerBound, qreal upperBound )
{
    setBoundaries( QskIntervalF( lowerBound, upperBound ) );
}

void QskScaleRenderer::setBoundaries( const QskIntervalF& boundaries )
{
    m_data->boundaries = boundaries;
}

QskIntervalF QskScaleRenderer::boundaries() const
{
    return m_data->boundaries;
}

qreal QskScaleRenderer::position() const
{
    return m_data->position;
}

void QskScaleRenderer::setPosition( qreal pos )
{
    m_data->position = pos;
}

void QskScaleRenderer::setRange( qreal from, qreal to )
{
    setRange( QskIntervalF( from, to ) );
}

void QskScaleRenderer::setRange( const QskIntervalF& range )
{
    m_data->range = range;
}

QskIntervalF QskScaleRenderer::range() const
{
    return m_data->range;
}

void QskScaleRenderer::setTickmarks( const QskTickmarks& tickmarks )
{
    m_data->tickmarks = tickmarks;
}

const QskTickmarks& QskScaleRenderer::tickmarks() const
{
    return m_data->tickmarks;
}

void QskScaleRenderer::setSpacing( qreal spacing )
{
    m_data->spacing = qMax( spacing, 0.0 );
}

qreal QskScaleRenderer::spacing() const
{
    return m_data->spacing;
}

void QskScaleRenderer::setTickColor( const QColor& color )
{
    m_data->tickColor = color;
}

QColor QskScaleRenderer::tickColor() const
{
    return m_data->tickColor;
}

void QskScaleRenderer::setTickLength( qreal length )
{
    m_data->tickLength = qMax( length, 0.0 );
}

qreal QskScaleRenderer::tickLength() const
{
    return m_data->tickLength;
}

void QskScaleRenderer::setTickWidth( qreal width )
{
    m_data->tickWidth = qMax( width, 0.0 );
}

qreal QskScaleRenderer::tickWidth() const
{
    return m_data->tickWidth;
}

void QskScaleRenderer::setFont( const QFont& font )
{
    m_data->font = font;
}

QFont QskScaleRenderer::font() const
{
    return m_data->font;
}

void QskScaleRenderer::setTextColors( const QskTextColors& textColors )
{
    m_data->textColors = textColors;
}

QskTextColors QskScaleRenderer::textColors() const
{
    return m_data->textColors;
}

void QskScaleRenderer::setColorFilter( const QskColorFilter& colorFilter )
{
    m_data->colorFilter = colorFilter;
}

const QskColorFilter& QskScaleRenderer::colorFilter() const
{
    return m_data->colorFilter;
}

QSGNode* QskScaleRenderer::updateNode(
    const QskSkinnable* skinnable, QSGNode* node )
{
    enum Role : quint8 { Ticks = 1, Labels = 2 };
    static const QVector< quint8 > roles = { Ticks, Labels };

    const auto transform = qskScaleTransform(
        m_data->edge, m_data->boundaries, m_data->range );

    if ( node == nullptr )
        node = new QSGNode();

    for ( auto role : roles )
    {
        auto oldNode = QskSGNode::findChildNode( node, role );

        auto newNode = ( role == Ticks )
            ? updateTicksNode( transform, oldNode )
            : updateLabelsNode( skinnable, transform, oldNode );

        QskSGNode::replaceChildNode( roles, role, node, oldNode, newNode );
    }

    return node;
}

QSGNode* QskScaleRenderer::updateTicksNode(
    const QTransform& transform, QSGNode* node ) const
{
    QskIntervalF backbone;
    if ( m_data->flags & Backbone )
        backbone = m_data->boundaries;

    const auto orientation = qskIsHorizontal( m_data->edge )
        ? Qt::Horizontal : Qt::Vertical;

    auto alignment = QskAxisScaleNode::Centered;

    if ( !( m_data->flags & CenteredTickmarks ) )
    {
        switch( m_data->edge )
        {
            case Qt::LeftEdge:
            case Qt::TopEdge:
                alignment = QskAxisScaleNode::Leading;
                break;
            case Qt::BottomEdge:
            case Qt::RightEdge:
                alignment = QskAxisScaleNode::Trailing;
                break;
        }
    }

    auto axisNode = QskSGNode::ensureNode< QskAxisScaleNode >( node );

    axisNode->setColor( m_data->tickColor );
    axisNode->setAxis( orientation, m_data->position, transform );
    axisNode->setTickGeometry( alignment, m_data->tickLength, m_data->tickWidth );
    axisNode->setPixelAlignment( Qt::Horizontal | Qt::Vertical );

    axisNode->update( m_data->tickmarks, backbone );

    return axisNode;
}

QSGNode* QskScaleRenderer::updateLabelsNode( const QskSkinnable* skinnable,
    const QTransform& transform, QSGNode* node ) const
{
    const auto ticks = m_data->tickmarks.majorTicks();
    if ( ticks.isEmpty() )
        return nullptr;

    if( node == nullptr )
        node = new QSGNode;

    const QFontMetricsF fm( m_data->font );

    auto nextNode = node->firstChild();

    QRectF lastRect; // to skip overlapping label

    for ( auto tick : ticks )
    {
        const auto label = labelAt( tick );

        const auto role = qskLabelNodeRole( label );

        if ( nextNode && QskSGNode::nodeRole( nextNode ) != role )
            nextNode = qskRemoveTraillingNodes( node, nextNode );

        QSizeF size;

        if ( label.canConvert< QString >() )
        {
            size = qskTextRenderSize( fm, label.toString() );
        }
        else if ( label.canConvert< QskGraphic >() )
        {
            const auto graphic = label.value< QskGraphic >();
            if ( !graphic.isNull() )
            {
                size.rheight() = fm.height();
                size.rwidth() = graphic.widthForHeight( size.height() );
            }
        }

        if ( size.isEmpty() )
            continue;

        const auto rect = labelRect( transform, tick, size );

        if ( !lastRect.isEmpty() && lastRect.intersects( rect ) )
        {
            /*
                Label do overlap: in case it is the last tick we remove
                the precessor - otherwise we simply skip this one
             */

            if ( tick != ticks.last() )
                continue; // skip this label

            if ( auto obsoleteNode = nextNode
                ? nextNode->previousSibling() : node->lastChild() )
            {
                node->removeChildNode( obsoleteNode );
                if ( obsoleteNode->flags() & QSGNode::OwnedByParent )
                    delete obsoleteNode;
            }
        }

        nextNode = updateTickLabelNode( skinnable, nextNode, label, rect );

        if ( nextNode)
        {
            lastRect = rect;

            if ( nextNode->parent() != node )
            {
                QskSGNode::setNodeRole( nextNode, role );
                node->appendChildNode( nextNode );
            }

            nextNode = nextNode->nextSibling();
        }
    }

    qskRemoveTraillingNodes( node, nextNode );

    return node;
}

QVariant QskScaleRenderer::labelAt( qreal pos ) const
{
    return QString::number( pos, 'g' );
}

// should be cached
QSizeF QskScaleRenderer::boundingLabelSize() const
{
    QSizeF boundingSize( 0.0, 0.0 );

    const auto ticks = m_data->tickmarks.majorTicks();
    if ( ticks.isEmpty() )
        return boundingSize;

    const QFontMetricsF fm( m_data->font );

    const qreal h = fm.height();

    for ( auto tick : ticks )
    {
        const auto label = labelAt( tick );
        if ( label.isNull() )
            continue;

        if ( label.canConvert< QString >() )
        {
            boundingSize = boundingSize.expandedTo(
                qskTextRenderSize( fm, label.toString() ) );
        }
        else if ( label.canConvert< QskGraphic >() )
        {
            const auto graphic = label.value< QskGraphic >();
            if ( !graphic.isNull() )
            {
                const auto w = graphic.widthForHeight( h );
                boundingSize.setWidth( qMax( boundingSize.width(), w ) );
            }
        }
    }

    return boundingSize;
}

QRectF QskScaleRenderer::labelRect(
    const QTransform& transform, qreal tick, const QSizeF& labelSize ) const
{
    const auto isHorizontal = qskIsHorizontal( m_data->edge );

    auto offset = m_data->tickLength + m_data->spacing;
    if ( m_data->flags & CenteredTickmarks )
        offset -= 0.5 * m_data->tickLength;

    const bool clampLabels = m_data->flags & ClampedLabels;

    const qreal w = labelSize.width();
    const qreal h = labelSize.height();

    qreal x, y;

    const ScaleMap map( isHorizontal, transform );

    const auto tickPos = map.map( tick );

    qreal min, max;
    if ( clampLabels )
    {
        min = map.map( m_data->boundaries.lowerBound() );
        max = map.map( m_data->boundaries.upperBound() );
    }

    if( isHorizontal )
    {
        x = tickPos - 0.5 * w;

        if ( clampLabels )
            x = qBound( min, x, max - w );

        y = m_data->position + offset;
    }
    else
    {
        const auto tickPos = map.map( tick );
        y = tickPos - 0.5 * h;

        if ( clampLabels )
            y = qBound( max, y, min - h );

        x = m_data->position - offset - w;
    }

    return QRectF( x, y, w, h );
}

QSGNode* QskScaleRenderer::updateTickLabelNode( const QskSkinnable* skinnable,
    QSGNode* node, const QVariant& label, const QRectF& rect ) const
{
    if ( label.canConvert< QString >() )
    {
        return QskSkinlet::updateTextNode( skinnable, node,
            rect, Qt::AlignCenter, label.toString(), m_data->font,
            QskTextOptions(), m_data->textColors, Qsk::Normal );
    }

    if ( label.canConvert< QskGraphic >() )
    {
        const auto alignment = qskIsHorizontal( m_data->edge )
            ? ( Qt::AlignHCenter | Qt::AlignBottom )
            : ( Qt::AlignRight | Qt::AlignVCenter );

        return QskSkinlet::updateGraphicNode(
            skinnable, node, label.value< QskGraphic >(),
            m_data->colorFilter, rect, alignment );
    }

    return nullptr;
}

#include "moc_QskScaleRenderer.cpp"
