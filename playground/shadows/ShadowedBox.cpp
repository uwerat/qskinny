/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShadowedBox.h"

#include <QskGradient.h>
#include <QskShadowMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>

ShadowedBox::ShadowedBox( QQuickItem* parentItem )
    : QskBox( true, parentItem )
{
    QColor c( Qt::darkRed );
#if 0
    c.setAlpha( 100 );
#endif

    setGradientHint( Panel, c );
    setBoxShapeHint( Panel, QskBoxShapeMetrics( 40, 0, 15, 0 ) );

    setBoxBorderMetricsHint( Panel, 0 );

#if 0
    setBoxBorderMetricsHint( Panel, 10 );
    setBoxBorderColorsHint( Panel, Qt::blue );
#endif

    setShadowColorHint( Panel, Qt::black );
}

ShadowedBox::~ShadowedBox()
{
}

void ShadowedBox::setOffsetX( qreal dx )
{
    auto metrics = shadowMetrics();
    metrics.setOffsetX( dx );

    setShadowMetrics( metrics );
}

qreal ShadowedBox::offsetX() const
{
    return shadowMetrics().offset().x();
}

void ShadowedBox::setOffsetY( qreal dy )
{
    auto metrics = shadowMetrics();
    metrics.setOffsetY( dy );

    setShadowMetrics( metrics );
}

qreal ShadowedBox::offsetY() const
{
    return shadowMetrics().offset().y();
}

void ShadowedBox::setSpreadRadius( qreal radius )
{
    auto metrics = shadowMetrics();
    metrics.setSpreadRadius( radius );

    setShadowMetrics( metrics );
}

qreal ShadowedBox::spreadRadius() const
{
    return shadowMetrics().spreadRadius();
}

void ShadowedBox::setBlurRadius( qreal radius )
{
    auto metrics = shadowMetrics();
    metrics.setBlurRadius( radius );

    setShadowMetrics( metrics );
}

qreal ShadowedBox::blurRadius() const
{
    return shadowMetrics().blurRadius();
}

void ShadowedBox::setOpacity( qreal opacity )
{
    opacity = qBound( 0.0, opacity, 1.0 );

    auto color = shadowColorHint( Panel );
    color.setAlphaF( opacity );

    setShadowColorHint( Panel, color );
}

qreal ShadowedBox::opacity() const
{
    return shadowColorHint( Panel ).alphaF();
}

QskShadowMetrics ShadowedBox::shadowMetrics() const
{
    return shadowMetricsHint( Panel );
}

void ShadowedBox::setShadowMetrics( const QskShadowMetrics& metrics )
{
    setShadowMetricsHint( Panel, metrics );
}

#include "moc_ShadowedBox.cpp"
