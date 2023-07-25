/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressBar.h"

#include "QskIntervalF.h"

QSK_SUBCONTROL( QskProgressBar, Groove )
QSK_SUBCONTROL( QskProgressBar, Fill )

class QskProgressBar::PrivateData
{
  public:
    Qt::Orientation orientation;
};

QskProgressBar::QskProgressBar( Qt::Orientation orientation,
        qreal min, qreal max, QQuickItem* parent )
    : Inherited( min, max, parent )
    , m_data( new PrivateData )
{
    m_data->orientation = orientation;

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::MinimumExpanding );

    setSubcontrolProxy( Inherited::Groove, Groove );
    setSubcontrolProxy( Inherited::Fill, Fill );
}

QskProgressBar::QskProgressBar( Qt::Orientation orientation, QQuickItem* parent )
    : QskProgressBar( orientation, 0.0, 100.0, parent )
{
}

QskProgressBar::QskProgressBar( const QskIntervalF& boundaries, QQuickItem* parent )
    : QskProgressBar( boundaries.lowerBound(), boundaries.upperBound(), parent )
{
}

QskProgressBar::QskProgressBar( qreal min, qreal max, QQuickItem* parent )
    : QskProgressBar( Qt::Horizontal, min, max, parent )
{
}

QskProgressBar::QskProgressBar( QQuickItem* parent )
    : QskProgressBar( Qt::Horizontal, parent )
{
}

QskProgressBar::~QskProgressBar()
{
}

QskAspect::Variation QskProgressBar::effectiveVariation() const
{
    // so you can define different hints depending on the orientation
    return static_cast< QskAspect::Variation >( m_data->orientation );
}

Qt::Orientation QskProgressBar::orientation() const
{
    return m_data->orientation;
}

void QskProgressBar::setOrientation( Qt::Orientation orientation )
{
    if ( orientation != m_data->orientation )
    {
        m_data->orientation = orientation;
        setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );

        resetImplicitSize();
        update();

        Q_EMIT orientationChanged( m_data->orientation );
    }
}

#include "moc_QskProgressBar.cpp"
