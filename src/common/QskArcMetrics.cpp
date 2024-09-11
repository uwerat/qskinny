/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcMetrics.h"
#include "QskFunctions.h"

#include <qhashfunctions.h>
#include <qpainterpath.h>
#include <qvariant.h>

static void qskRegisterArcMetrics()
{
    qRegisterMetaType< QskArcMetrics >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskArcMetrics >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterArcMetrics )

static inline QPainterPath qskRadialPathPath(
    const QRectF& rect, qreal startAngle, qreal spanAngle, qreal width )
{
    const auto sz = qMin( rect.width(), rect.height() );

    const auto tx = width * rect.width() / sz;
    const auto ty = width * rect.height() / sz;

    const auto innerRect = rect.adjusted( tx, ty, -tx, -ty );

    QPainterPath path;

    if ( innerRect.isEmpty() )
    {
        if ( qAbs( spanAngle ) >= 360.0 )
        {
            path.addEllipse( rect );
        }
        else
        {
            // pie
            path.arcMoveTo( rect, startAngle );
            path.arcTo( rect, startAngle, spanAngle );
            path.lineTo( rect.center() );
            path.closeSubpath();
        }
    }
    else
    {
        if ( qAbs( spanAngle ) >= 360.0 )
        {
            path.addEllipse( rect );

            QPainterPath innerPath;
            innerPath.addEllipse( innerRect );
            path -= innerPath;
        }
        else
        {
            /*
                We need the end point of the inner arc to add the line that connects
                the inner/outer arcs. As QPainterPath does not offer such a method
                we insert a dummy arcMoveTo and grab the calculated position.
             */
            path.arcMoveTo( innerRect, startAngle + spanAngle );
            const auto pos = path.currentPosition();

            path.arcMoveTo( rect, startAngle ); // replaces the dummy arcMoveTo above
            path.arcTo( rect, startAngle, spanAngle );

            path.lineTo( pos );
            path.arcTo( innerRect, startAngle + spanAngle, -spanAngle );

            path.closeSubpath();
        }
    }

    return path;
}

static inline QPainterPath qskOrthogonalPath(
    const QRectF& rect, qreal startAngle, qreal spanAngle, qreal width )
{
    const auto t2 = 0.5 * width;
    const auto r = rect.adjusted( t2, t2, -t2, -t2 );

    QPainterPath arcPath;
    arcPath.arcMoveTo( r, startAngle );
    arcPath.arcTo( r, startAngle, spanAngle );

    QPainterPathStroker stroker;
    stroker.setCapStyle( Qt::FlatCap );
    stroker.setWidth( width );

    return stroker.createStroke( arcPath );
}

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

static inline qreal qskEffectiveThickness( qreal radius, qreal percentage )
{
    return qMax( radius, 0.0 ) * qBound( 0.0, percentage, 100.0 ) / 100.0;
}

void QskArcMetrics::setThickness( qreal thickness ) noexcept
{
    m_thickness = thickness;
}

void QskArcMetrics::setStartAngle( qreal startAngle ) noexcept
{
    m_startAngle = qskConstrainedDegrees( startAngle );
}

void QskArcMetrics::setSpanAngle( qreal spanAngle ) noexcept
{
    m_spanAngle = qBound( -360.0, spanAngle, 360.0 );
}

void QskArcMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_relativeSize = ( sizeMode == Qt::RelativeSize );
}

bool QskArcMetrics::isClosed() const
{
    return qAbs( m_spanAngle ) >= 360.0;
}

bool QskArcMetrics::containsAngle( qreal angle ) const
{
    angle = qskConstrainedDegrees( angle );

    if ( m_spanAngle >= 0.0 )
    {
        if ( angle < m_startAngle )
            angle += 360.0;

        return ( angle >= m_startAngle ) && ( angle <= m_startAngle + m_spanAngle );
    }
    else
    {
        if ( angle > m_startAngle )
            angle -= 360.0;

        return ( angle <= m_startAngle ) && ( angle >= m_startAngle + m_spanAngle );
    }
}

QskArcMetrics QskArcMetrics::interpolated(
    const QskArcMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_relativeSize != to.m_relativeSize ) )
        return to;

    const qreal thickness = qskInterpolated( m_thickness, to.m_thickness, ratio );

    const qreal s1 = qskInterpolated( m_startAngle, to.m_startAngle, ratio );
    const qreal s2 = qskInterpolated( endAngle(), to.endAngle(), ratio );

    return QskArcMetrics( s1, s2 - s1, thickness, sizeMode() );
}

QVariant QskArcMetrics::interpolate(
    const QskArcMetrics& from, const QskArcMetrics& to,
    qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

QskArcMetrics QskArcMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    return toAbsolute( 0.5 * size.width(), 0.5 * size.height() );
}

QskArcMetrics QskArcMetrics::toAbsolute( qreal radiusX, qreal radiusY ) const noexcept
{
    if ( radiusX < 0.0 )
        return toAbsolute( radiusY );

    if ( radiusY < 0.0 )
        return toAbsolute( radiusX );

    return toAbsolute( qMin( radiusX, radiusY ) );
}

QskArcMetrics QskArcMetrics::toAbsolute( qreal radius ) const noexcept
{
    if ( !m_relativeSize )
        return *this;

    const qreal t = qskEffectiveThickness( radius, m_thickness );
    return QskArcMetrics( m_startAngle, m_spanAngle, t, Qt::AbsoluteSize );
}

QPainterPath QskArcMetrics::painterPath( const QRectF& rect, bool radial ) const
{
    QPainterPath path;

    if ( !qFuzzyIsNull( m_spanAngle ) )
    {
        qreal t = m_thickness;

        if ( m_relativeSize )
        {
            const auto sz = qMin( rect.width(), rect.height() );
            t = qskEffectiveThickness( 0.5 * sz, t );
        }

        if ( t > 0.0 )
        {
            if ( radial )
                path = qskRadialPathPath( rect, m_startAngle, m_spanAngle, t );
            else
                path = qskOrthogonalPath( rect, m_startAngle, m_spanAngle, t );
        }
    }

    return path;
}

QRectF QskArcMetrics::boundingRect( const QRectF& ellipseRect ) const
{
    if ( qFuzzyIsNull( m_spanAngle ) )
        return QRectF( ellipseRect.center(), QSizeF() );

    if ( qAbs( m_spanAngle ) >= 360.0 )
        return ellipseRect;

    return painterPath( ellipseRect ).controlPointRect();
}

QSizeF QskArcMetrics::boundingSize( const QSizeF& ellipseSize ) const
{
    if ( qFuzzyIsNull( m_spanAngle ) )
        return QSizeF();

    if ( qAbs( m_spanAngle ) >= 360.0 )
        return ellipseSize;

    const QRectF r( 0.0, 0.0, ellipseSize.width(), ellipseSize.height() );
    return painterPath( r ).controlPointRect().size();
}

QskHashValue QskArcMetrics::hash( QskHashValue seed ) const noexcept
{
    auto hash = qHash( m_thickness, seed );
    hash = qHash( m_startAngle, hash );
    hash = qHash( m_spanAngle, hash );

    return qHash( m_relativeSize, hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskArcMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskArcMetrics" << '(';
    debug << metrics.thickness() << ',' << metrics.sizeMode();
    debug << ",[" << metrics.startAngle() << ',' << metrics.spanAngle() << ']';
    debug << ')';

    return debug;
}

#endif

#include "moc_QskArcMetrics.cpp"
