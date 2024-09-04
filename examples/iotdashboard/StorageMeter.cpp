/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "StorageMeter.h"

#include <QskFontRole.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( StorageMeter, Status )

StorageMeter::StorageMeter( QQuickItem* parent ) noexcept
    : QskProgressRing( parent )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Constrained );

    m_label = new QskTextLabel( this );
    m_label->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    m_label->setLayoutAlignmentHint( Qt::AlignCenter );
    m_label->setFontRole( QskFontRole::Caption );

    connect( this, &QskProgressRing::valueChanged,
        this, &StorageMeter::updateMeter );

    updateMeter( value() );
}

void StorageMeter::updateMeter( const qreal value )
{
    const auto color = qskInterpolatedColorAt(
        gradientHint( Status ).stops(), value / 100.0 );

    setFillGradient( { color, color.lighter() } );

    m_label->setTextColor( color );

    const auto locale = this->locale();
    const auto text = locale.toString( static_cast< int >( value ) )
        + " " + locale.percent();

    m_label->setText( text );
}

QSizeF StorageMeter::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    qreal size;

    if ( constraint.width() > 0 )
    {
        size = constraint.width();
    }
    else if ( constraint.height() > 0 )
    {
        size = constraint.height();
    }
    else
    {
        size = 57;
    }

    return QSizeF( size, size );
}
