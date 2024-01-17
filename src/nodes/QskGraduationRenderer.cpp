/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGraduationRenderer.h"
#include "QskGraduationMetrics.h"
#include "QskTickmarks.h"
#include "QskSkinlet.h"
#include "QskSGNode.h"
#include "QskGraduationNode.h"
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

class QskGraduationRenderer::PrivateData
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

    QskGraduationMetrics metrics = { 4, 6, 8 };
    qreal spacing = 5.0;

    QFont font;
    QskTextColors textColors;

    QskColorFilter colorFilter;

    Qt::Edge edge = Qt::BottomEdge;
    QskGraduationRenderer::Flags flags = ClampedLabels;
};

QskGraduationRenderer::QskGraduationRenderer()
    : m_data( new PrivateData() )
{
}

QskGraduationRenderer::~QskGraduationRenderer()
{
}

void QskGraduationRenderer::setEdge( Qt::Edge edge )
{
    m_data->edge = edge;
}

Qt::Edge QskGraduationRenderer::edge() const
{
    return m_data->edge;
}

void QskGraduationRenderer::setFlag( Flag flag, bool on )
{
    if ( on )
        m_data->flags |= flag;
    else
        m_data->flags &= ~flag;
}

void QskGraduationRenderer::setFlags( Flags flags )
{
    m_data->flags = flags;
}

QskGraduationRenderer::Flags QskGraduationRenderer::flags() const
{
    return m_data->flags;
}

void QskGraduationRenderer::setBoundaries( qreal lowerBound, qreal upperBound )
{
    setBoundaries( QskIntervalF( lowerBound, upperBound ) );
}

void QskGraduationRenderer::setBoundaries( const QskIntervalF& boundaries )
{
    m_data->boundaries = boundaries;
}

QskIntervalF QskGraduationRenderer::boundaries() const
{
    return m_data->boundaries;
}

qreal QskGraduationRenderer::position() const
{
    return m_data->position;
}

void QskGraduationRenderer::setPosition( qreal pos )
{
    m_data->position = pos;
}

void QskGraduationRenderer::setRange( qreal from, qreal to )
{
    setRange( QskIntervalF( from, to ) );
}

void QskGraduationRenderer::setRange( const QskIntervalF& range )
{
    m_data->range = range;
}

QskIntervalF QskGraduationRenderer::range() const
{
    return m_data->range;
}

void QskGraduationRenderer::setTickmarks( const QskTickmarks& tickmarks )
{
    m_data->tickmarks = tickmarks;
}

const QskTickmarks& QskGraduationRenderer::tickmarks() const
{
    return m_data->tickmarks;
}

void QskGraduationRenderer::setSpacing( qreal spacing )
{
    m_data->spacing = qMax( spacing, 0.0 );
}

qreal QskGraduationRenderer::spacing() const
{
    return m_data->spacing;
}

void QskGraduationRenderer::setTickColor( const QColor& color )
{
    m_data->tickColor = color;
}

QColor QskGraduationRenderer::tickColor() const
{
    return m_data->tickColor;
}

void QskGraduationRenderer::setTickMetrics( const QskGraduationMetrics& metrics )
{
    m_data->metrics = metrics;
}

const QskGraduationMetrics& QskGraduationRenderer::tickMetrics() const
{
    return m_data->metrics;
}

void QskGraduationRenderer::setFont( const QFont& font )
{
    m_data->font = font;
}

QFont QskGraduationRenderer::font() const
{
    return m_data->font;
}

void QskGraduationRenderer::setTextColors( const QskTextColors& textColors )
{
    m_data->textColors = textColors;
}

QskTextColors QskGraduationRenderer::textColors() const
{
    return m_data->textColors;
}

void QskGraduationRenderer::setColorFilter( const QskColorFilter& colorFilter )
{
    m_data->colorFilter = colorFilter;
}

const QskColorFilter& QskGraduationRenderer::colorFilter() const
{
    return m_data->colorFilter;
}

QSGNode* QskGraduationRenderer::updateNode(
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

QSGNode* QskGraduationRenderer::updateTicksNode(
    const QTransform& transform, QSGNode* node ) const
{
    QskIntervalF backbone;
    if ( m_data->flags & Backbone )
        backbone = m_data->boundaries;

    const auto orientation = qskIsHorizontal( m_data->edge )
        ? Qt::Horizontal : Qt::Vertical;

    auto alignment = QskGraduationNode::Centered;

    if ( !( m_data->flags & CenteredTickmarks ) )
    {
        switch( m_data->edge )
        {
            case Qt::LeftEdge:
            case Qt::TopEdge:
                alignment = QskGraduationNode::Leading;
                break;
            case Qt::BottomEdge:
            case Qt::RightEdge:
                alignment = QskGraduationNode::Trailing;
                break;
        }
    }

    auto graduationNode = QskSGNode::ensureNode< QskGraduationNode >( node );

    graduationNode->setColor( m_data->tickColor );
    graduationNode->setAxis( orientation, m_data->position, transform );
    graduationNode->setTickMetrics( alignment, m_data->metrics );
    graduationNode->setPixelAlignment( Qt::Horizontal | Qt::Vertical );

    graduationNode->update( m_data->tickmarks, backbone );

    return graduationNode;
}

QSGNode* QskGraduationRenderer::updateLabelsNode( const QskSkinnable* skinnable,
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

QVariant QskGraduationRenderer::labelAt( qreal pos ) const
{
    return QString::number( pos, 'g' );
}

// should be cached
QSizeF QskGraduationRenderer::boundingLabelSize() const
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

QRectF QskGraduationRenderer::labelRect(
    const QTransform& transform, qreal tick, const QSizeF& labelSize ) const
{
    const auto isHorizontal = qskIsHorizontal( m_data->edge );

    const auto tickLength = m_data->metrics.maxLength();

    auto offset = tickLength + m_data->spacing;
    if ( m_data->flags & CenteredTickmarks )
        offset -= 0.5 * tickLength;

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

QSGNode* QskGraduationRenderer::updateTickLabelNode( const QskSkinnable* skinnable,
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

#include "moc_QskGraduationRenderer.cpp"
