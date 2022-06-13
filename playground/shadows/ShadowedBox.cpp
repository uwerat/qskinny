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
}

ShadowedBox::~ShadowedBox()
{
}

void ShadowedBox::setShadow( const QskShadowMetrics& shadow )
{
    setShadowMetricsHint( Panel, shadow );
}

void ShadowedBox::setShadowColor( const QColor& color )
{
    setShadowColorHint( Panel, color );
}

void ShadowedBox::setGradient( const QskGradient& gradient )
{
    setGradientHint( Panel, gradient );
}

void ShadowedBox::setShape( const QskBoxShapeMetrics& shape )
{
    setBoxShapeHint( Panel, shape );
}

void ShadowedBox::setBorderWidth( qreal width )
{
    setBoxBorderMetricsHint( Panel, width );
}

void ShadowedBox::setBorderColors( const QskBoxBorderColors& colors )
{
    setBoxBorderColorsHint( Panel, colors );
}

#include "moc_ShadowedBox.cpp"
