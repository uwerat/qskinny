/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSwitchButton.h"

QSK_SUBCONTROL( QskSwitchButton, Handle )
QSK_SUBCONTROL( QskSwitchButton, Groove )

struct QskSwitchButton::PrivateData
{
    PrivateData( Qt::Orientation orientation )
        : orientation( orientation )
    {
    }

    bool inverted = false;
    Qt::Orientation orientation;
};

QskSwitchButton::QskSwitchButton( QQuickItem* parent )
    : QskSwitchButton( Qt::Horizontal, parent )
{
}

QskSwitchButton::QskSwitchButton( Qt::Orientation orientation, QQuickItem* parent )
    : QskAbstractButton( parent )
    , m_data( new PrivateData( orientation ) )
{
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskSwitchButton::~QskSwitchButton()
{
}

bool QskSwitchButton::isCheckable() const
{
    return true;
}

Qt::Orientation QskSwitchButton::orientation() const
{
    return m_data->orientation;
}

void QskSwitchButton::setOrientation( Qt::Orientation orientation )
{
    if( m_data->orientation != orientation )
    {
        m_data->orientation = orientation;

        resetImplicitSize();
        update();

        Q_EMIT orientationChanged( orientation );
    }
}

bool QskSwitchButton::isInverted() const
{
    return m_data->inverted;
}

void QskSwitchButton::setInverted( bool on )
{
    if( m_data->inverted != on )
    {
        m_data->inverted = on;

        resetImplicitSize(); // in case the size hints depend on it
        update();

        Q_EMIT invertedChanged( on );
    }
}

QskAspect::Variation QskSwitchButton::effectiveVariation() const
{
    return static_cast< QskAspect::Variation >( m_data->orientation );
}

#include "moc_QskSwitchButton.cpp"
