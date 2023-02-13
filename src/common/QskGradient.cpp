/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradient.h"
#include "QskRgbValue.h"
#include "QskGradientDirection.h"
#include "QskFunctions.h"

#include <qvariant.h>
#include <qdebug.h>

static void qskRegisterGradient()
{
    qRegisterMetaType< QskGradient >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskGradient >();
#endif

    QMetaType::registerConverter< QColor, QskGradient >(
        []( const QColor& color ) { return QskGradient( color ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterGradient )

static inline bool qskIsGradientValid( const QskGradientStops& stops )
{
    if ( stops.isEmpty() )
        return false;

    if ( stops.first().position() < 0.0 || stops.last().position() > 1.0 )
        return false;

    if ( !stops.first().color().isValid() )
        return false;

    for ( int i = 1; i < stops.size(); i++ )
    {
        if ( stops[ i ].position() < stops[ i - 1 ].position() )
            return false;

        if ( !stops[ i ].color().isValid() )
            return false;
    }

    return true;
}

static inline bool qskCanBeInterpolated( const QskGradient& from, const QskGradient& to )
{
    if ( from.isMonochrome() || to.isMonochrome() )
        return true;

    return from.type() == to.type();
}

static inline QTransform qskTransformForRect( int, const QRectF& rect )
{
    const qreal x = rect.x();
    const qreal y = rect.y();
    const qreal w = rect.width();
    const qreal h = rect.height();

    return QTransform( w, 0, 0, h, x, y );
}
    
QskGradient::QskGradient( const QColor& color )
    : QskGradient()
{
    setStops( color );
}

QskGradient::QskGradient( const QColor& color1, const QColor& color2 )
    : QskGradient()
{
    setStops( color1, color2 );
}

QskGradient::QskGradient( QGradient::Preset preset )
    : QskGradient()
{
    setStops( qskBuildGradientStops( QGradient( preset ).stops() ) );
}

QskGradient::QskGradient( const QVector< QskGradientStop >& stops )
    : QskGradient()
{
    setStops( stops );
}

QskGradient::QskGradient( const QGradient& qGradient )
    : QskGradient()
{
    switch( qGradient.type() )
    {
        case QGradient::LinearGradient:
        {
            m_type = Linear;

            const auto g = static_cast< const QLinearGradient* >( &qGradient );

            m_values[0] = g->start().x();
            m_values[1] = g->start().y();
            m_values[2] = g->finalStop().x();
            m_values[3] = g->finalStop().y();

            break;
        }
        case QGradient::RadialGradient:
        {
            m_type = Radial;

            const auto g = static_cast< const QRadialGradient* >( &qGradient );

            if ( ( g->center() != g->focalPoint() ) || ( g->radius() != g->focalRadius() ) )
                qWarning() << "QskGradient: extended radial gradients are not supported.";

            m_values[0] = g->focalPoint().x();
            m_values[1] = g->focalPoint().y();
            m_values[3] = m_values[2] = g->focalRadius();

            break;
        }
        case QGradient::ConicalGradient:
        {
            m_type = Conic;

            const auto g = static_cast< const QConicalGradient* >( &qGradient );

            m_values[0] = g->center().x();
            m_values[1] = g->center().y();
            m_values[2] = g->angle();
            m_values[3] = 360.0;

            break;
        }
        default:
        {
            m_type = Stops;
            break;
        }
    }

    m_spreadMode = static_cast< SpreadMode >( qGradient.spread() );

    switch( qGradient.coordinateMode() )
    {
        case QGradient::ObjectMode:
        case QGradient::ObjectBoundingMode:

            m_stretchMode = StretchToSize;
            break;

        case QGradient::LogicalMode:

            m_stretchMode = NoStretch;
            break;

        case QGradient::StretchToDeviceMode:
        {
            qWarning() << "QskGradient: StretchToDeviceMode is not supportd.";
            m_stretchMode = NoStretch;
        }
    }

    setStops( qskBuildGradientStops( qGradient.stops() ) );
}

QskGradient::QskGradient( const QskGradient& other ) noexcept
    : m_stops( other.m_stops )
    , m_values{ other.m_values[0], other.m_values[1],
        other.m_values[2], other.m_values[3], }
    , m_type( other.m_type )
    , m_spreadMode( other.m_spreadMode )
    , m_stretchMode( other.m_stretchMode )
    , m_isDirty( other.m_isDirty )
    , m_isValid( other.m_isValid )
    , m_isMonchrome( other.m_isMonchrome )
    , m_isVisible( other.m_isVisible )
{
}

QskGradient::~QskGradient()
{
}

QskGradient& QskGradient::operator=( const QskGradient& other ) noexcept
{
    m_type = other.m_type;
    m_spreadMode = other.m_spreadMode;
    m_stretchMode = other.m_stretchMode;
    m_stops = other.m_stops;

    m_values[0] = other.m_values[0];
    m_values[1] = other.m_values[1];
    m_values[2] = other.m_values[2];
    m_values[3] = other.m_values[3];

    m_isDirty = other.m_isDirty;
    m_isValid = other.m_isValid;
    m_isMonchrome = other.m_isMonchrome;
    m_isVisible = other.m_isVisible;

    return *this;
}

bool QskGradient::operator==( const QskGradient& other ) const noexcept
{
    return ( m_type == other.m_type )
           && ( m_spreadMode == other.m_spreadMode )
           && ( m_stretchMode == other.m_stretchMode )
           && ( m_values[0] == other.m_values[0] )
           && ( m_values[1] == other.m_values[1] )
           && ( m_values[2] == other.m_values[2] )
           && ( m_values[3] == other.m_values[3] )
           && ( m_stops == other.m_stops );
}

void QskGradient::updateStatusBits() const
{
    // doing all bits in one loop ?
    m_isValid = qskIsGradientValid( m_stops );

    if ( m_isValid )
    {
        m_isMonchrome = qskIsMonochrome( m_stops );
        m_isVisible = qskIsVisible( m_stops );
    }
    else
    {
        m_isMonchrome = true;
        m_isVisible = false;
    }

    if ( m_isVisible )
    {
        switch( m_type )
        {
            case Linear:
            {
                m_isVisible = !( qskFuzzyCompare( m_values[0], m_values[2] )
                    && qskFuzzyCompare( m_values[1], m_values[3] ) );
                break;
            }

            case Radial:
            {
                m_isVisible = m_values[2] > 0.0 || m_values[3] > 0.0; // radius
                break;
            }

            case Conic:
            {
                m_isVisible = !qFuzzyIsNull( m_values[3] ); // spanAngle
                break;
            }

            default:
                break;
        }
    }

    m_isDirty = false;
}

bool QskGradient::isValid() const noexcept
{
    if ( m_isDirty )
        updateStatusBits();

    return m_isValid;
}

bool QskGradient::isMonochrome() const noexcept
{
    if ( m_isDirty )
        updateStatusBits();

    return m_isMonchrome;
}

bool QskGradient::isVisible() const noexcept
{
    if ( m_isDirty )
        updateStatusBits();

    return m_isVisible;
}

void QskGradient::setStops( const QColor& color )
{
    m_stops = { { 0.0, color }, { 1.0, color } };
    m_isDirty = true;
}

void QskGradient::setStops( const QColor& color1, const QColor& color2 )
{
    m_stops = { { 0.0, color1 }, { 1.0, color2 } };
    m_isDirty = true;
}

void QskGradient::setStops( QGradient::Preset preset )
{
    const auto stops = qskBuildGradientStops( QGradient( preset ).stops() );
    setStops( stops );
}

void QskGradient::setStops( const QskGradientStops& stops )
{
    if ( !stops.isEmpty() && !qskIsGradientValid( stops ) )
    {
        qWarning( "Invalid gradient stops" );
        m_stops.clear();
    }
    else
    {
        m_stops = stops;
    }

    m_isDirty = true;
}

int QskGradient::stepCount() const noexcept
{
    if ( !isValid() )
        return 0;

    int steps = m_stops.count() - 1;

    if ( m_stops.first().position() > 0.0 )
        steps++;

    if ( m_stops.last().position() < 1.0 )
        steps++;

    return steps;
}

qreal QskGradient::stopAt( int index ) const noexcept
{
    if ( index < 0 || index >= m_stops.size() )
        return -1.0;

    return m_stops[ index ].position();
}

bool QskGradient::hasStopAt( qreal value ) const noexcept
{
    // better use binary search TODO ...
    for ( auto& stop : m_stops )
    {
        if ( stop.position() == value )
            return true;

        if ( stop.position() > value )
            break;
    }

    return false;
}

QColor QskGradient::colorAt( int index ) const noexcept
{
    if ( index >= m_stops.size() )
        return QColor();

    return m_stops[ index ].color();
}

void QskGradient::setAlpha( int alpha )
{
    for ( auto& stop : m_stops )
    {
        auto c = stop.color();
        if ( c.isValid() && c.alpha() )
        {
            c.setAlpha( alpha );
            stop.setColor( c );
        }
    }

    m_isDirty = true;
}

void QskGradient::setSpreadMode( SpreadMode spreadMode )
{
    m_spreadMode = spreadMode;
}

void QskGradient::setStretchMode( StretchMode stretchMode )
{
    m_stretchMode = stretchMode;
}

void QskGradient::stretchTo( const QRectF& rect )
{
    if ( m_stretchMode == NoStretch || m_type == Stops || rect.isEmpty() )
        return; // nothing to do

    const auto transform = qskTransformForRect( m_stretchMode, rect );

    switch( static_cast< int >( m_type ) )
    {
        case Linear:
        {
            transform.map( m_values[0], m_values[1], &m_values[0], &m_values[1] );
            transform.map( m_values[2], m_values[3], &m_values[2], &m_values[3] );

            break;
        }
        case Radial:
        {
            transform.map( m_values[0], m_values[1], &m_values[0], &m_values[1] );

            qreal rx = qMax( m_values[2], 0.0 );
            qreal ry = qMax( m_values[3], 0.0 );

            if ( rx == 0.0 || ry == 0.0 )
            {
                /*
                    It would be more logical if the scaling happens according
                    the width, when rx is set ad v.v. But fitting the circle is
                    probably, what most use cases need - and how to specify
                    this. Maybe by introducing another stretchMode ... TODO
                 */
                const qreal r = qMin( rect.width(), rect.height() ) * qMax( rx, ry );
                m_values[2] = m_values[3] = r;
            }
            else
            {
                m_values[2] = rx * rect.width();
                m_values[3] = ry * rect.height();
            }

            break;
        }
        case Conic:
        {
            transform.map( m_values[0], m_values[1], &m_values[0], &m_values[1] );
            break;
        }
    }

    m_stretchMode = NoStretch;
}

QskGradient QskGradient::stretchedTo( const QSizeF& size ) const
{
    return stretchedTo( QRectF( 0.0, 0.0, size.width(), size.height() ) );
}

QskGradient QskGradient::stretchedTo( const QRectF& rect ) const
{
    if ( m_stretchMode == NoStretch )
        return *this;

    QskGradient g = *this;
    g.stretchTo( rect );

    return g;
}

void QskGradient::reverse()
{
    if ( isMonochrome() )
        return;

    std::reverse( m_stops.begin(), m_stops.end() );
    for( auto& stop : m_stops )
        stop.setPosition( 1.0 - stop.position() );
}

QskGradient QskGradient::reversed() const
{
    auto gradient = *this;
    gradient.reverse();

    return gradient;
}

QskGradient QskGradient::extracted( qreal from, qreal to ) const
{
    auto gradient = *this;

    if ( !isValid() || ( from > to ) || ( from > 1.0 ) )
    {
        gradient.clearStops();
    }
    else if ( isMonochrome() )
    {
        from = qMax( from, 0.0 );
        to = qMin( to, 1.0 );

        const auto color = m_stops.first().color();

        gradient.setStops( { { from, color }, { to, color } } );
    }
    else
    {
        gradient.setStops( qskExtractedGradientStops( m_stops, from, to ) );
    }

    return gradient;
}

QskGradient QskGradient::interpolated( const QskGradient& to, qreal ratio ) const
{
    if ( !isValid() && !to.isValid() )
        return to;

    QskGradient gradient;

    if ( qskCanBeInterpolated( *this, to ) )
    {
        // We simply interpolate stops and values

        gradient = to;

        const auto stops = qskInterpolatedGradientStops(
            m_stops, isMonochrome(), to.m_stops, to.isMonochrome(), ratio );

        gradient.setStops( stops );

        for ( uint i = 0; i < sizeof( m_values ) / sizeof( m_values[0] ); i++ )
            gradient.m_values[i] = m_values[i] + ratio * ( to.m_values[i] - m_values[i] );
    }
    else
    {
        /*
            The interpolation is devided into 2 steps. First we
            interpolate into a monochrome gradient and then
            recolor the gradient towards the target gradient
            This will always result in a smooth transition - even, when
            interpolating between different gradient types
         */

        const auto c = QskRgb::interpolated( startColor(), to.startColor(), 0.5 );

        if ( ratio < 0.5 )
        {
            const auto r = 2.0 * ratio;

            gradient = *this;
            gradient.setStops( qskInterpolatedGradientStops( m_stops, c, r ) );
        }
        else
        {
            const auto r = 2.0 * ( ratio - 0.5 );

            gradient = to;
            gradient.setStops( qskInterpolatedGradientStops( c, to.m_stops, r ) );
        }
    }

    return gradient;
}

QVariant QskGradient::interpolate(
    const QskGradient& from, const QskGradient& to, qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

void QskGradient::clearStops()
{
    if ( !m_stops.isEmpty() )
    {
        m_stops.clear();
        m_isDirty = true;
    }
}

QskHashValue QskGradient::hash( QskHashValue seed ) const
{
    auto hash = qHash( m_type, seed );
    hash = qHash( m_spreadMode, seed );
    hash = qHash( m_stretchMode, seed );

    if ( m_type != Stops )
        hash = qHashBits( m_values, sizeof( m_values ), hash );

    for ( const auto& stop : m_stops )
        hash = stop.hash( hash );

    return hash;
}

void QskGradient::setLinearDirection( Qt::Orientation orientation )
{
    setLinearDirection( QskLinearDirection( orientation ) );
}

void QskGradient::setLinearDirection( qreal x1, qreal y1, qreal x2, qreal y2 )
{
    setLinearDirection( QskLinearDirection( x1, y1, x2, y2 ) );
}

void QskGradient::setLinearDirection( const QskLinearDirection& direction )
{
    m_type = Linear;

    m_values[0] = direction.x1();
    m_values[1] = direction.y1();
    m_values[2] = direction.x2();
    m_values[3] = direction.y2();
}

QskLinearDirection QskGradient::linearDirection() const
{
    Q_ASSERT( m_type == Linear );

    if ( m_type != Linear )
        return QskLinearDirection( 0.0, 0.0, 0.0, 0.0 );

    return QskLinearDirection( m_values[0], m_values[1], m_values[2], m_values[3] );
}

void QskGradient::setRadialDirection( const qreal x, qreal y, qreal radius )
{
    setRadialDirection( QskRadialDirection( x, y, radius ) );
}

void QskGradient::setRadialDirection( const qreal x, qreal y,qreal radiusX, qreal radiusY )
{
    setRadialDirection( QskRadialDirection( x, y, radiusX, radiusY ) );
}
void QskGradient::setRadialDirection( const QskRadialDirection& direction )
{
    m_type = Radial;

    m_values[0] = direction.center().x();
    m_values[1] = direction.center().y();
    m_values[2] = direction.radiusX();
    m_values[3] = direction.radiusY();
}

QskRadialDirection QskGradient::radialDirection() const
{
    Q_ASSERT( m_type == Radial );

    if ( m_type != Radial )
        return QskRadialDirection( 0.5, 0.5, 0.0 );

    return QskRadialDirection( m_values[0], m_values[1], m_values[2], m_values[3] );
}

void QskGradient::setConicDirection( qreal x, qreal y )
{
    setConicDirection( QskConicDirection( x, y ) );
}

void QskGradient::setConicDirection( qreal x, qreal y,
    qreal startAngle, qreal spanAngle )
{
    setConicDirection( QskConicDirection( x, y, startAngle, spanAngle ) );
}

void QskGradient::setConicDirection( const QskConicDirection& direction )
{
    m_type = Conic;

    m_values[0] = direction.center().x();
    m_values[1] = direction.center().y();
    m_values[2] = direction.startAngle();
    m_values[3] = direction.spanAngle();
}

QskConicDirection QskGradient::conicDirection() const
{
    Q_ASSERT( m_type == Conic );

    if ( m_type != Conic )
        return QskConicDirection( 0.5, 0.5, 0.0, 0.0 );

    return QskConicDirection( m_values[0], m_values[1], m_values[2], m_values[3] );
}

void QskGradient::setDirection( Type type )
{
    switch( type )
    {
        case Linear:
            setLinearDirection( QskLinearDirection() );
            break;

        case Radial:
            setRadialDirection( QskRadialDirection() );
            break;

        case Conic:
            setConicDirection( QskConicDirection() );
            break;

        case Stops:
            resetDirection();
            break;
    }
}

void QskGradient::resetDirection()
{
    m_type = Stops;
    m_values[0] = m_values[1] = m_values[2] = m_values[3] = 0.0;
}

QskGradient QskGradient::effectiveGradient() const
{
    if ( ( m_type == QskGradient::Stops ) || isMonochrome() )
    {
        // the shader for linear gradients is the fastest

        QskGradient g = *this;
        g.setDirection( QskGradient::Linear );

        return g;
    }

    return *this;
}

QGradient QskGradient::toQGradient() const
{
    QGradient g;

    switch( static_cast< int >( m_type ) )
    {
        case Linear:
        {
            g = QLinearGradient( m_values[0], m_values[1], m_values[2], m_values[3] );
            break;
        }

        case Radial:
        {
            g = QRadialGradient( m_values[0], m_values[1], m_values[2] );
            break;
        }

        case Conic:
        {
            if ( m_values[3] != 360.0 )
            {
                qWarning() <<
                    "QskGradient: spanAngle got lost, when converting to QConicalGradient";
            }

            g = QConicalGradient( m_values[0], m_values[1], m_values[2] );
            break;
        }
    }

    g.setCoordinateMode( m_stretchMode == NoStretch 
        ? QGradient::LogicalMode : QGradient::ObjectMode );

    g.setSpread( static_cast< QGradient::Spread >( m_spreadMode ) );
    g.setStops( qskToQGradientStops( m_stops ) );

    return g;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGradient& gradient )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskGradient(";

    switch ( gradient.type() )
    {
        case QskGradient::Linear:
        {
            debug << " L(";

            const auto dir = gradient.linearDirection();
            debug << dir.start().x() << "," << dir.start().y()
                << "," << dir.stop().x() << "," << dir.stop().y() << ")";

            break;
        }

        case QskGradient::Radial:
        {
            debug << " R(";

            const auto dir = gradient.radialDirection();

            debug << dir.center().x() << "," << dir.center().y()
                << "," << dir.radiusX() << dir.radiusY() << ")";

            break;
        }

        case QskGradient::Conic:
        {
            debug << " C(";

            const auto dir = gradient.conicDirection();

            debug << dir.center().x() << "," << dir.center().y()
                << ",[" << dir.startAngle() << "," << dir.spanAngle() << "])";
            break;
        }

        case QskGradient::Stops:
        {
            break;
        }
    }

    if ( !gradient.stops().isEmpty() )
    {
        if ( gradient.isMonochrome() )
        {
            debug << ' ';
            QskRgb::debugColor( debug, gradient.rgbStart() );
        }
        else
        {
            debug << " ( ";

            const auto& stops = gradient.stops();
            for ( int i = 0; i < stops.count(); i++ )
            {
                if ( i != 0 )
                    debug << ", ";

                debug << stops[i];
            }

            debug << " )";
        }
    }

    if ( gradient.stretchMode() == QskGradient::StretchToSize )
    {
        debug << " SS";
    }

    switch( gradient.spreadMode() )
    {
        case QskGradient::RepeatSpread:
            debug << " RP";
            break;

        case QskGradient::ReflectSpread:
            debug << " RF";
            break;

        case QskGradient::PadSpread:
            break;
    }

    debug << " )";

    return debug;
}

#endif

#include "moc_QskGradient.cpp"
