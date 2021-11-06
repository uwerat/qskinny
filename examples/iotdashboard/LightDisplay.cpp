/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightDisplay.h"
#include "Skin.h"

#include <QskAnimator.h>
#include <QskRgbValue.h>
#include <QskSetup.h>
#include <QskTextLabel.h>
#include <QskQuick.h>

#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QRadialGradient>

QSK_SUBCONTROL( LightDisplay, Panel )
QSK_SUBCONTROL( LightDisplay, Groove )
QSK_SUBCONTROL( LightDisplay, ColdAndWarmArc )
QSK_SUBCONTROL( LightDisplay, ValueText )
QSK_SUBCONTROL( LightDisplay, LeftLabel )
QSK_SUBCONTROL( LightDisplay, RightLabel )

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskBoundedValueInput( parent )
{
    setAlignmentHint( LeftLabel, Qt::AlignRight );
    setAlignmentHint( ValueText, Qt::AlignCenter | Qt::AlignHCenter );

    setBoundaries( 0, 100 );

    // ### move to Skin:
    setShadow( { 0, 20 } );
    setShadowColor( 0xe5e5e5 );
}

void LightDisplay::setShadow( const QskShadowMetrics& shadow )
{
    m_shadow = shadow;
    update();
}

const QskShadowMetrics& LightDisplay::shadow() const
{
    return m_shadow;
}

void LightDisplay::setGradient( const QskGradient& gradient )
{
    m_gradient = gradient;
    update();
}

const QskGradient& LightDisplay::gradient() const
{
    return m_gradient;
}

void LightDisplay::setShadowColor( const QColor& color )
{
    m_shadowColor = color;
    update();
}

QColor LightDisplay::shadowColor() const
{
    return m_shadowColor;
}

#include "moc_LightDisplay.cpp"
