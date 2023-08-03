/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <QskFunctions.h>
#include <QskLevelingSensor.h>
#include <QskScaleTickmarks.h>

#include <type_traits>
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

    template<>
    bool compareExchange< QVector3D >( QVector3D& dst, const QVector3D& src )
    {
        auto dirty = false;
        dirty |= compareExchange(dst[Qt::XAxis], src[Qt::XAxis]);
        dirty |= compareExchange(dst[Qt::YAxis], src[Qt::YAxis]);
        dirty |= compareExchange(dst[Qt::ZAxis], src[Qt::ZAxis]);
        return dirty;
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

class QskLevelingSensor::PrivateData
{
  public:
    QVector3D m_angles = { 45, 45, 45 };
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

void QskLevelingSensor::setAngle( const QVector3D& degrees )
{
    if ( compareExchange( m_data->m_angles, degrees ) )
    {
        update();
        Q_EMIT anglesChanged( m_data->m_angles );
    }
}

QskScaleTickmarks QskLevelingSensor::tickmarks( const Qt::Axis axis ) const
{
    if ( isAxis( axis ) )
    {
        return m_data->m_tickmarks[ axis ];
    }
    static const QskScaleTickmarks invalid;
    return invalid;
}

QskLevelingSensor::TickmarksLabels QskLevelingSensor::tickmarkLabels(
    const Qt::Axis axis ) const
{
    if ( isAxis( axis ) )
    {
        return m_data->m_tickmarksLabels[ axis ];
    }
    static const QskLevelingSensor::TickmarksLabels invalid;
    return invalid;
}

QVector3D QskLevelingSensor::angles() const
{
    return m_data->m_angles;
}

QVector3D QskLevelingSensor::subControlRotation(
    const QskAspect::Subcontrol subControl ) const
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
    const QskAspect::Subcontrol subControl, const QVector3D& degrees )
{
    auto updateSubControlRotation = [ this ]( const QskAspect::Subcontrol subControl,
                                        const QVector3D& degrees ) {
        if ( compareExchange( m_data->m_subControlRotation[ subControl ], degrees ) )
        {
            Q_EMIT subControlRotationChanged( subControl, degrees );
            update();
        }
    };

    using Q = QskLevelingSensor;

    if ( subControl == Q::TickmarksX || subControl == Q::TickmarksXLabels )
    {
        updateSubControlRotation( Q::TickmarksX, degrees );
        updateSubControlRotation( Q::TickmarksXLabels, degrees );
    }
    else if ( subControl == Q::TickmarksY || subControl == Q::TickmarksYLabels )
    {
        updateSubControlRotation( Q::TickmarksY, degrees );
        updateSubControlRotation( Q::TickmarksYLabels, degrees );
    }
    else if ( subControl == Q::TickmarksZ || subControl == TickmarksZLabels )
    {
        updateSubControlRotation( TickmarksZ, degrees );
        updateSubControlRotation( TickmarksZLabels, degrees );
    }
    else
    {
        updateSubControlRotation( subControl, degrees );
    }
}

#include "moc_QskLevelingSensor.cpp"