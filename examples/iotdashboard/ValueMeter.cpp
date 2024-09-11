/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ValueMeter.h"

#include <QskFontRole.h>
#include <QskTextLabel.h>

ValueMeter::ValueMeter( QQuickItem* parent )
    : QskProgressRing( parent )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Constrained );

    m_label = new QskTextLabel( this );
    m_label->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    m_label->setLayoutAlignmentHint( Qt::AlignCenter );
    m_label->setFontRole( QskFontRole::Caption );

    connect( this, &QskProgressRing::valueChanged,
        this, &ValueMeter::updateMeter );

    updateMeter( value() );
}

void ValueMeter::updateMeter( const qreal value )
{
    m_label->setText( text( value ) );
}

QString ValueMeter::text( qreal value ) const
{
    value = static_cast< int >( value );
    return locale().toString( value ) + ' ' + locale().percent();
}

void ValueMeter::setTextColor( const QColor& color )
{
    m_label->setTextColor( color );
}
