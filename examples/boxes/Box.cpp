/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Box.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskHctColor.h>
#include <QskRgbValue.h>

static inline void setStartStop( Box::FillType type, QskGradient& gradient )
{
    qreal x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;

    if ( type != Box::Horizontal )
        y2 = 1.0;

    if ( type != Box::Vertical )
        x2 = 1.0;

    gradient.setLinearDirection( x1, y1, x2, y2 );
}

Box::Box( QQuickItem* parentItem )
    : QskBox( parentItem )
{
    setSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Ignored );

    // decoupling the hints from the skin
    setBoxShapeHint( QskBox::Panel, QskBoxShapeMetrics() );
    setBoxBorderMetricsHint( QskBox::Panel, QskBoxBorderMetrics() );
    setBoxBorderColorsHint( QskBox::Panel, QskBoxBorderColors() );
    setPanelGradient( QskGradient() );
}

void Box::setWebGradient( FillType type, QGradient::Preset preset, bool inverted )
{
    if ( type == Unfilled )
    {
        setPanelGradient( QskGradient() );
        return;
    }

    QskGradient gradient( preset );

    if ( type == Solid )
    {
        const auto color = QskRgb::interpolated(
            gradient.startColor(), gradient.endColor(), 0.5 );

        gradient.setStops( color );
    }

    setStartStop( type, gradient );

    if ( inverted )
        gradient.reverse();

    setPanelGradient( gradient );
}

void Box::setTonalGradient( FillType type, const QRgb base, bool inverted )
{
    if ( type == Unfilled )
    {
        setPanelGradient( QskGradient() );
        return;
    }

    QskGradient gradient;

    const QskHctColor htcColor( base );
    if ( type == Solid )
    {
        gradient.setStops( htcColor.toned( 50 ).rgb() );
    }
    else if ( type != Unfilled )
    {
        gradient.setStops( htcColor.toned( 70 ).rgb(),
            htcColor.toned( 40 ).rgb() );
    }

    setStartStop( type, gradient );

    if ( inverted )
        gradient.reverse();

    setPanelGradient( gradient );
}

void Box::setTonalPalette( FillType type, const QRgb base )
{
    if ( type == Unfilled || type == Solid )
    {
        setTonalGradient( type, base );
        return;
    }

    QskGradient gradient;
    setStartStop( type, gradient );

    {
        const QskHctColor hctColor( base );

        QVector< QRgb > colors;
        colors.reserve( 10 );

        for ( int i = 0; i < 10; i++ )
            colors += hctColor.toned( 90 - i * 7 ).rgb();

        gradient.setStops( qskBuildGradientStops( colors, true ) );
    }

    setPanelGradient( gradient );
}

void Box::setBorder( BorderType type, const QRgb base )
{
    setBorderWidth( 5 );

    switch ( static_cast< int >( type ) )
    {
        case NoBorder:
            setBorderWidth( 0 );
            return;

        case Flat:
            setBorderGradient( base );
            return;
    }

    const QskHctColor htcColor( base );

    const auto dark = htcColor.toned( 40 ).rgb();
    const auto mid = htcColor.toned( 65 ).rgb();
    const auto light = htcColor.toned( 90 ).rgb();

    switch ( static_cast< int >( type ) )
    {
        case Raised1:
            setBorderGradients( light, light, dark, dark );
            break;

        case Sunken1:
            setBorderGradients( dark, dark, light, light );
            break;

        case Raised2:
            setBorderGradients( mid, light, mid, dark );
            break;

        case Sunken2:
            setBorderGradients( mid, dark, mid, light );
            break;
    }
}

void Box::setShape( const QskBoxShapeMetrics& shape )
{
    setBoxShapeHint( QskBox::Panel, shape );
}

void Box::setShape( qreal radius, Qt::SizeMode sizeMode )
{
    setBoxShapeHint( QskBox::Panel, QskBoxShapeMetrics( radius, sizeMode ) );
}

void Box::setShape( qreal radiusX, qreal radiusY, Qt::SizeMode sizeMode )
{
    setBoxShapeHint( QskBox::Panel,
        QskBoxShapeMetrics( radiusX, radiusY, sizeMode ) );
}

void Box::setBorderGradients( const QskGradient& left, const QskGradient& top,
    const QskGradient& right, const QskGradient& bottom )
{
    QskBoxBorderColors colors( left, top, right, bottom );
    setBoxBorderColorsHint( QskBox::Panel, colors );
}

void Box::setBorderGradient( const QskGradient& gradient )
{
    setBoxBorderColorsHint( QskBox::Panel, gradient );
}

void Box::setBorderWidth( qreal left, qreal top, qreal right, qreal bottom )
{
    setBoxBorderMetricsHint( QskBox::Panel,
        QskMargins( left, top, right, bottom ) );
}

void Box::setBorderWidth( int width )
{
    setBoxBorderMetricsHint( QskBox::Panel, QskMargins( width ) );
}

void Box::setGradient( QRgb rgb )
{
    setGradient( QColor::fromRgba( rgb ) );
}

void Box::setGradient( Qt::GlobalColor color )
{
    setGradient( QColor( color ) );
}

void Box::setGradient( const QColor& color )
{
    setPanelGradient( QskGradient( color ) );
}

void Box::setGradient( FillType fillType,
    const QColor& color1, const QColor& color2 )
{
    QskGradientStops stops;
    stops += QskGradientStop( 0.0, color1 );
    stops += QskGradientStop( 1.0, color2 );
    
    setGradient( fillType, stops );
}

void Box::setGradient( FillType fillType,
    const QColor& color1, const QColor& color2, const QColor& color3 )
{
    QskGradientStops stops;
    stops += QskGradientStop( 0.0, color1 );
    stops += QskGradientStop( 0.5, color2 );
    stops += QskGradientStop( 1.0, color3 );

    setGradient( fillType, stops );
}

void Box::setGradient( FillType fillType, const QskGradientStops& stops )
{
    QskGradient gradient( stops );
    setStartStop( fillType, gradient );

    setPanelGradient( gradient );
}

void Box::setPanelGradient( const QskGradient& gradient )
{
    setGradientHint( QskBox::Panel, gradient );
}
