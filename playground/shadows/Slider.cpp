/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Slider.h"

#include <QskSlider.h>
#include <QskTextLabel.h>
#include <QskPlainTextRenderer.h>

#include <qfont.h>

Slider::Slider( const QString& text, qreal min, qreal max,
        qreal step, qreal value, QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, parent )
{
    m_label = new QskTextLabel( text, this );
    m_label->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

    m_slider = new QskSlider( this );
    m_slider->setBoundaries( min, max );
    m_slider->setStepSize( step );
    m_slider->setSnapping( true );
    m_slider->setTickPolicy( Qsk::Never ); // too many steps
    m_slider->setValue( value );

    m_valueLabel = new QskTextLabel( this );
    m_valueLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    updateLabel( value );

    const auto sz = QskPlainTextRenderer::textSize(
        QStringLiteral( "-100" ), m_valueLabel->font() );
    m_valueLabel->setFixedWidth( sz.width() );

    connect( m_slider, &QskSlider::valueChanged, this, &Slider::updateLabel );
    connect( m_slider, &QskSlider::valueChanged, this, &Slider::valueChanged );
}

void Slider::updateLabel( qreal value )
{
    m_valueLabel->setText( QString::number( value ) );
}

void Slider::setValue( qreal value )
{
    m_slider->setValue( value );
}

#include "moc_Slider.cpp"
