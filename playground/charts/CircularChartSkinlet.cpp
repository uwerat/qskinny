/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "CircularChartSkinlet.h"
#include "CircularChart.h"
#include "ChartSample.h"

#include <QskIntervalF.h>
#include <QskArcMetrics.h>
#include <QskArcNode.h>

#include <qpainterpath.h>
#include <qmath.h>

#define PAINTED_NODE 0

#if PAINTED_NODE

/*
    This is a fallback implementation for a user who is using an outdated
    version of QSkinny where only shaders for linear gradients are available.
 */
#include <QskPaintedNode.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qbrush.h>

namespace
{
    QConicalGradient qskQConicalGradient(
        const QskGradientStops& stops, qreal startAngle, qreal spanAngle )
    {
        QskGradientStops scaledStops;
        scaledStops.reserve( stops.size() );

        const auto ratio = qAbs( spanAngle ) / 360.0;

        if ( spanAngle > 0.0 )
        {
            for ( auto it = stops.cbegin(); it != stops.cend(); ++it )
                scaledStops += { ratio * it->position(), it->color() };
        }
        else
        {
            for ( auto it = stops.crbegin(); it != stops.crend(); ++it )
                scaledStops += { 1.0 - ratio * it->position(), it->color() };
        }

        QConicalGradient qGradient( QPointF(), startAngle );
        qGradient.setStops( qskToQGradientStops( scaledStops ) );

        return qGradient;
    }

    class PaintedArcNode : public QskPaintedNode
    {
      public:
        void setArcData( const QRectF&, const QskArcMetrics&,
            qreal, const QColor&, const QskGradient&, QQuickWindow* );

      protected:
        void paint( QPainter*, const QSize&, const void* nodeData ) override;
        QskHashValue hash( const void* nodeData ) const override;

      private:
        QskHashValue arcHash( const QRectF&, const QskArcMetrics&,
            qreal, const QColor&, const QskGradient& ) const;

        QBrush fillBrush( const QskGradient&, const QRectF&, qreal, qreal ) const;

        struct ArcData
        {
            QPointF translation;
            QPen pen;
            QBrush brush;
            QPainterPath path;

            QskHashValue hash;
        };
    };

    void PaintedArcNode::setArcData(
        const QRectF& rect, const QskArcMetrics& metrics,
        qreal borderWidth, const QColor& borderColor,
        const QskGradient& gradient, QQuickWindow* window )
    {
        const auto hash = arcHash( rect, metrics, borderWidth, borderColor, gradient );

        const auto brush = fillBrush( gradient, rect,
            metrics.startAngle(), metrics.spanAngle() );

        QPen pen( borderColor, borderWidth );
        if ( borderWidth <= 0.0 )
            pen.setStyle( Qt::NoPen );

        const auto path = metrics.painterPath( rect );
        const auto r = path.controlPointRect();

        const ArcData arcData { r.topLeft(), pen, brush, path, hash };
        update( window, r, QSizeF(), &arcData );
    }

    void PaintedArcNode::paint( QPainter* painter, const QSize&, const void* nodeData )
    {
        const auto arcData = reinterpret_cast< const ArcData* >( nodeData );

        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->translate( -arcData->translation );
        painter->setPen( arcData->pen );
        painter->setBrush( arcData->brush );
        painter->drawPath( arcData->path );
    }

    QskHashValue PaintedArcNode::hash( const void* nodeData ) const
    {
        const auto arcData = reinterpret_cast< const ArcData* >( nodeData );
        return arcData->hash;
    }

    QBrush PaintedArcNode::fillBrush( const QskGradient& gradient,
        const QRectF& rect, qreal startAngle, qreal spanAngle ) const
    {
        const auto qGradient = qskQConicalGradient(
            gradient.stops(), startAngle, spanAngle );

        const qreal sz = qMax( rect.width(), rect.height() );
        const qreal sx = rect.width() / sz;
        const qreal sy = rect.height() / sz;

        QTransform t;
        t.scale( sx, sy );
        t.translate( rect.center().x() / sx, rect.center().y() / sy );

        QBrush brush( qGradient );
        brush.setTransform( t );

        return brush;
    }

    inline QskHashValue PaintedArcNode::arcHash(
        const QRectF& rect, const QskArcMetrics& metrics, qreal borderWidth,
        const QColor& borderColor, const QskGradient& gradient ) const
    {
        auto hash = metrics.hash( 6753 );
        hash = qHashBits( &rect, sizeof( rect ), hash );
        hash = qHash( borderWidth, hash );
        hash = qHash( borderColor.rgba(), hash );
        hash = gradient.hash( hash );

        return hash;
    }
}

#endif // PAINTED_NODE

namespace
{
    inline QskArcMetrics segmentMetrics(
        const QskSkinnable* skinnable, int index )
    {
        auto metrics = skinnable->arcMetricsHint( CircularChart::Arc );

        const auto chart = static_cast< const CircularChart* >( skinnable );

        const auto span = chart->stackedInterval( index );
        const auto total = chart->effectiveTotalValue();

        const auto a1 = metrics.angleAtRatio( span.lowerBound() / total );
        const auto a2 = metrics.angleAtRatio( span.upperBound() / total );

        metrics.setStartAngle( a1 );
        metrics.setSpanAngle( a2 - a1 );

        return metrics;
    }

    QRectF closedArcRect( const QskSkinnable* skinnable )
    {
        using Q = CircularChart;

        const auto chart = static_cast< const CircularChart* >( skinnable );
        const auto r = chart->subControlContentsRect( Q::Arc );

        const auto metrics = skinnable->arcMetricsHint( Q::Arc );
        if ( metrics.isClosed() )
            return r;

        const auto size = skinnable->strutSizeHint( CircularChart::Arc );

        auto pos = r.topLeft();

        {
            const QRectF ellipseRect( 0.0, 0.0, size.width(), size.height() );
            pos -= metrics.boundingRect( ellipseRect ).topLeft();
        }

        return QRectF( pos.x(), pos.y(), size.width(), size.height() );
    }

    static inline QRectF textRect( qreal x, qreal y )
    {
        const qreal sz = 1000.0; // something big enough to avoid wrapping
        return QRectF( x - 0.5 * sz, y - 0.5 * sz, sz, sz );
    }
}

class CircularChartSkinlet::PrivateData
{
  public:
    // caching the geometry of the rect for drawing all arc segments
    mutable QRectF closedArcRect;
};

CircularChartSkinlet::CircularChartSkinlet( QskSkin* skin )
    : Inherited( skin )
    , m_data( new PrivateData() )
{
    setNodeRoles( { PanelRole, ArcRole, SegmentRole, SegmentLabelRole } );
}

CircularChartSkinlet::~CircularChartSkinlet() = default;

void CircularChartSkinlet::updateNode(
    QskSkinnable* skinnable, QSGNode* node ) const
{
    m_data->closedArcRect = ::closedArcRect( skinnable );
    Inherited::updateNode( skinnable, node );
}

QRectF CircularChartSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = CircularChart;

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Arc )
    {
        const auto chart = static_cast< const CircularChart* >( skinnable );
        return chart->subControlContentsRect( Q::Panel );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* CircularChartSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = CircularChart;

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case ArcRole:
        {
            const auto aspect = Q::Arc | QskAspect::Border;

            const auto borderColor = skinnable->color( aspect );
            const auto borderWidth = skinnable->metric( aspect );

            return updateArcSegmentNode( skinnable, node, borderWidth, borderColor,
                skinnable->gradientHint( Q::Arc ), skinnable->arcMetricsHint( Q::Arc ) );
        }

        case SegmentRole:
            //return updateSeriesNode( skinnable, Q::Segment, node );
            return Inherited::updateSubNode( skinnable, nodeRole, node );

        case SegmentLabelRole:
            return updateSeriesNode( skinnable, Q::SegmentLabel, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int CircularChartSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    using Q = CircularChart;

    if ( subControl == Q::Segment || subControl == Q::SegmentLabel )
    {
        const auto chart = static_cast< const CircularChart* >( skinnable );
        return chart->series().count();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QRectF CircularChartSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl, int index ) const
{
    // See https://github.com/uwerat/qskinny/issues/307
    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

int CircularChartSkinlet::sampleIndexAt(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, const QPointF& pos ) const
{
    // See https://github.com/uwerat/qskinny/issues/307
    return Inherited::sampleIndexAt( skinnable, contentsRect, subControl, pos );
}

QSGNode* CircularChartSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    using Q = CircularChart;

    const auto chart = static_cast< const CircularChart* >( skinnable );

     if ( subControl == Q::Segment )
     {
         const auto aspect = Q::Segment | QskAspect::Border;
     
         const auto borderColor = skinnable->color( aspect );
         const auto borderWidth = skinnable->metric( aspect );
     
         return updateArcSegmentNode( skinnable, node, borderWidth, borderColor,
             chart->gradientAt( index ), ::segmentMetrics( skinnable, index ) );
     }

    if ( subControl == Q::SegmentLabel )
    {
        return updateArcLabelNode( skinnable, node,
            chart->labelAt( index ), ::segmentMetrics( skinnable, index ) );
    }

    return nullptr;
}

QSGNode* CircularChartSkinlet::updateArcSegmentNode(
    const QskSkinnable* skinnable,
    QSGNode* node, qreal borderWidth, const QColor& borderColor,
    const QskGradient& gradient, const QskArcMetrics& metrics ) const
{
#if PAINTED_NODE
    auto arcNode = static_cast< PaintedArcNode* >( node );
    if ( arcNode == nullptr )
        arcNode = new PaintedArcNode();

    const auto chart = static_cast< const CircularChart* >( skinnable );

    arcNode->setArcData( m_data->closedArcRect, metrics,
        borderWidth, borderColor, gradient, chart->window() );
#else
    Q_UNUSED( skinnable )

    auto fillGradient = gradient;

    if ( fillGradient.type() == QskGradient::Stops )
    {
        fillGradient.setStretchMode( QskGradient::StretchToSize );
        fillGradient.setConicDirection( 0.5, 0.5,
            metrics.startAngle(), metrics.spanAngle() );
    }

    auto arcNode = static_cast< QskArcNode* >( node );
    if ( arcNode == nullptr )
        arcNode = new QskArcNode();

    arcNode->setArcData( m_data->closedArcRect, metrics,
        borderWidth, borderColor, fillGradient );
#endif

    return arcNode;
}

QSGNode* CircularChartSkinlet::updateArcLabelNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QString& text, const QskArcMetrics& metrics ) const
{
    /*
        possible improvements:

            - rotating the labels
            - elide/wrap modes
     */
    const auto& r = m_data->closedArcRect;
    const auto sz = qMin( r.width(), r.height() );

    if ( text.isEmpty() || sz <= 0.0 )
        return nullptr;

    const auto m = metrics.toAbsolute( 0.5 * sz );

    const auto pos = 0.5; // [0.0, 1.0] -> distance from the inner border
    const auto dt = pos * ( 1.0 - m.thickness() / sz );

    const qreal radians = qDegreesToRadians( m.startAngle() + 0.5 * m.spanAngle() );
    const auto c = r.center();

    const qreal x = c.x () + dt * r.width() * qFastCos( radians );
    const qreal y = c.y() - dt * r.height() * qFastSin( radians );

    return updateTextNode( skinnable, node,
        textRect( x, y ), Qt::AlignCenter, text, CircularChart::SegmentLabel );
}

QSizeF CircularChartSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    using Q = CircularChart;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto sz = skinnable->strutSizeHint( CircularChart::Arc );

    auto metrics = skinnable->arcMetricsHint( Q::Arc );

    QSizeF hint;

    if ( metrics.isClosed() )
    {
        // We only support constrained hints for closed arcs

        if ( constraint.width() >= 0.0 || constraint.height() >= 0.0 )
        {
            const qreal ratio = sz.isEmpty() ? 1.0 : sz.width() / sz.height();

            if ( constraint.width() >= 0.0 )
            {
                hint = skinnable->innerBoxSize(
                    Q::Panel, QSizeF( constraint.width(), constraint.width() ) );

                hint.setHeight( hint.width() / ratio );
                hint = skinnable->outerBoxSize( Q::Panel, hint );

                return QSizeF( -1.0, hint.height() );
            }
            else
            {
                hint = skinnable->innerBoxSize(
                    Q::Panel, QSizeF( constraint.height(), constraint.height() ) );

                hint.setWidth( hint.height() * ratio );
                hint = skinnable->outerBoxSize( Q::Panel, hint );

                return QSizeF( hint.width(), -1.0 );
            }
        }
    }

    if ( constraint.width() >= 0.0 || constraint.height() >= 0.0 )
        return QSizeF();

    metrics = metrics.toAbsolute( 0.5 * sz.width(), 0.5 * sz.height() );

    hint = metrics.boundingSize( sz );
    hint = skinnable->outerBoxSize( Q::Panel, hint );

    return hint;
}

#include "moc_CircularChartSkinlet.cpp"
