/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSeparator.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskSeparator, Panel )

QskSeparator::QskSeparator( QQuickItem* parent )
    : QskSeparator( Qt::Horizontal, parent )
{
}

QskSeparator::QskSeparator( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_orientation( orientation )
{
    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Minimum );
}

QskSeparator::~QskSeparator()
{
}

void QskSeparator::setOrientation( Qt::Orientation orientation )
{
    if ( orientation == m_orientation )
        return;

    m_orientation = orientation;

#if 1
    // swapping the size policy: guess this is what a user expects
    setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );
#endif

    resetImplicitSize();
    update();

    Q_EMIT orientationChanged( m_orientation );
}

Qt::Orientation QskSeparator::orientation() const
{
    return m_orientation;
}

void QskSeparator::setExtent( qreal extent )
{
    if ( extent < 0.0 )
        extent = 0.0;

    if ( setMetric( Panel | QskAspect::Size, extent ) )
        Q_EMIT extentChanged( extent );
}

void QskSeparator::resetExtent()
{
    if ( resetMetric( Panel | QskAspect::Size ) )
        Q_EMIT extentChanged( extent() );
}

qreal QskSeparator::extent() const
{
    return metric( Panel | QskAspect::Size );
}

QskAspect::Placement QskSeparator::effectivePlacement() const
{
    return static_cast< QskAspect::Placement >( m_orientation );
}

#include "moc_QskSeparator.cpp"
