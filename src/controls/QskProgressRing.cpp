/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressRing.h"

#include "QskIntervalF.h"

QSK_SUBCONTROL( QskProgressRing, Groove )
QSK_SUBCONTROL( QskProgressRing, Fill )

class QskProgressRing::PrivateData
{
  public:
    int size : 2;
};

QskProgressRing::QskProgressRing( qreal min, qreal max, QQuickItem* parent )
    : Inherited( min, max, parent )
    , m_data( new PrivateData )
{
    m_data->size = NormalSize;

    setSubcontrolProxy( Inherited::Groove, Groove );
    setSubcontrolProxy( Inherited::Fill, Fill );
}

QskProgressRing::QskProgressRing( QQuickItem* parent )
    : QskProgressRing( 0.0, 100.0, parent )
{
}

QskProgressRing::QskProgressRing( const QskIntervalF& boundaries, QQuickItem* parent )
    : QskProgressRing( boundaries.lowerBound(), boundaries.upperBound(), parent )
{
}

QskProgressRing::~QskProgressRing()
{
}

QskAspect::Variation QskProgressRing::effectiveVariation() const
{
    switch( size() )
    {
        case SmallSize:
            return QskAspect::Small;

        case LargeSize:
            return QskAspect::Large;

        default:
            return QskAspect::NoVariation;
    }
}

void QskProgressRing::setSize( Size size )
{
    if ( size != m_data->size )
    {
        m_data->size = size;

        resetImplicitSize();
        update();

        Q_EMIT sizeChanged( size );
    }
}

QskProgressRing::Size QskProgressRing::size() const
{
    return static_cast< Size >( m_data->size );
}

#include "moc_QskProgressRing.cpp"
