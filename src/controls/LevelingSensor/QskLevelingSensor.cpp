#include "QskLevelingSensor.h"
#include <QskFunctions.h>
#include <QskScaleTickmarks.h>
#include <unordered_map>

#include <QVector2D>
#include <QVector3D>

namespace
{
    template< typename T >
    bool compareExchange( T& dst, const T& src )
    {
        if ( dst != src )
        {
            dst = src;
            return true;
        }
        return false;
    }

    template<>
    bool compareExchange< float >( float& dst, const float& src )
    {
        if ( !qskFuzzyCompare( dst, src ) )
        {
            dst = src;
            return true;
        }
        return false;
    }

    inline bool isAxis( const Qt::Axis axis )
    {
        return axis == Qt::XAxis || axis == Qt::YAxis || axis == Qt::ZAxis;
    }
}

QSK_SUBCONTROL( QskLevelingSensor, OuterDisk )
QSK_SUBCONTROL( QskLevelingSensor, Horizon )
QSK_SUBCONTROL( QskLevelingSensor, TickmarksX )
QSK_SUBCONTROL( QskLevelingSensor, TickmarksXLabels )
QSK_SUBCONTROL( QskLevelingSensor, TickmarksY )
QSK_SUBCONTROL( QskLevelingSensor, TickmarksYLabels )
QSK_SUBCONTROL( QskLevelingSensor, TickmarksZ )
QSK_SUBCONTROL( QskLevelingSensor, TickmarksZLabels )

#define RETURN_IF_FALSE( expr )                                                                    \
    if ( !( expr ) )                                                                               \
        return;

using Q = QskLevelingSensor;

class QskLevelingSensor::PrivateData
{
  public:
    QVector3D m_angle = { 45, 45, 45 };
    Tickmarks m_tickmarks[ 3 ];
    TickmarksLabels m_tickmarksLabels[ 3 ];
    std::unordered_map< QskAspect::Subcontrol, QVector3D > m_subControlRotation;
};

QskLevelingSensor::QskLevelingSensor( QQuickItem* const parent )
    : Inherited( parent )
    , m_data( std::make_unique< QskLevelingSensor::PrivateData >() )
{
}

QskLevelingSensor::~QskLevelingSensor() = default;

void QskLevelingSensor::setTickmarks( const Qt::Axis axis, QskScaleTickmarks tickmarks )
{
    RETURN_IF_FALSE( isAxis( axis ) );
    m_data->m_tickmarks[ axis ] = std::move( tickmarks );
    update();
}

void QskLevelingSensor::setTickmarksLabels( const Qt::Axis axis, TickmarksLabels labels )
{
    RETURN_IF_FALSE( isAxis( axis ) );
    m_data->m_tickmarksLabels[ axis ] = std::move( labels );
    update();
}

void QskLevelingSensor::setAngle( const QVector3D& degree )
{
    if ( compareExchange( m_data->m_angle, degree ) )
    {
        update();
        Q_EMIT anglesChanged( m_data->m_angle );
    }
}

void QskLevelingSensor::setAngle( const Qt::Axis axis, const float degree )
{
    RETURN_IF_FALSE( isAxis( axis ) );

    if ( compareExchange( m_data->m_angle[ axis ], degree ) )
    {
        update();
        Q_EMIT anglesChanged( m_data->m_angle );
    }
}

const QskScaleTickmarks& QskLevelingSensor::tickmarks( Qt::Axis axis ) const
{
    if ( isAxis( axis ) )
    {
        return m_data->m_tickmarks[ axis ];
    }
    static const QskScaleTickmarks invalid;
    return invalid;
}

const QskLevelingSensor::TickmarksLabels& QskLevelingSensor::tickmarkLabels( Qt::Axis axis ) const
{
    if ( isAxis( axis ) )
    {
        return m_data->m_tickmarksLabels[ axis ];
    }
    static const QskLevelingSensor::TickmarksLabels invalid;
    return invalid;
}

const QVector3D& QskLevelingSensor::angle() const noexcept
{
    return m_data->m_angle;
}

const QVector3D& QskLevelingSensor::subControlRotation(
    const QskAspect::Subcontrol subControl ) const noexcept
{
    static const QVector3D notFound;
    const auto found = m_data->m_subControlRotation.find( subControl );
    if ( found == m_data->m_subControlRotation.end() )
    {
        return notFound;
    }
    return found->second;
}

void QskLevelingSensor::setSubControlRotation(
    const QskAspect::Subcontrol subControl, const QVector3D& degree )
{
    auto updateSubControlRotation = [ this ]( const QskAspect::Subcontrol subControl,
                                        const QVector3D& degree ) {
        if ( compareExchange( m_data->m_subControlRotation[ subControl ], degree ) )
        {
            Q_EMIT subControlRotationChanged( subControl, degree );
            update();
        }
    };

    if ( subControl == Q::TickmarksX || subControl == Q::TickmarksXLabels )
    {
        updateSubControlRotation( Q::TickmarksX, degree );
        updateSubControlRotation( Q::TickmarksXLabels, degree );
    }
    else if ( subControl == Q::TickmarksY || subControl == Q::TickmarksYLabels )
    {
        updateSubControlRotation( Q::TickmarksY, degree );
        updateSubControlRotation( Q::TickmarksYLabels, degree );
    }
    else if ( subControl == Q::TickmarksZ || subControl == TickmarksZLabels )
    {
        updateSubControlRotation( TickmarksZ, degree );
        updateSubControlRotation( TickmarksZLabels, degree );
    }
    else
    {
        updateSubControlRotation( subControl, degree );
    }
}

#include "moc_QskLevelingSensor.cpp"