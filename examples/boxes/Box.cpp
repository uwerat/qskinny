/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Box.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskHctColor.h>

Box::Box( QQuickItem* parentItem )
    : QskBox( parentItem )
{
    setSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Ignored );

    // decoupling the hints from the skin
    setBoxShapeHint( QskBox::Panel, QskBoxShapeMetrics() );
    setBoxBorderMetricsHint( QskBox::Panel, QskBoxBorderMetrics() );
    setBoxBorderColorsHint( QskBox::Panel, QskBoxBorderColors() );
    setGradientHint( QskBox::Panel, QskGradient() );
}

void Box::setBackground( FillType type, QGradient::Preset preset, bool inverted )
{
    if ( type == Unfilled )
    {
        setGradient( QskGradient() );
        return;
    }

    QskGradient gradient( preset );

    if ( type == Solid )
    {
        const auto& stops = gradient.stops();

        const auto color = QskGradientStop::interpolated(
            stops.first(), stops.last(), 0.5 );

        setGradient( QskGradient( color ) );
    }
    else
    {
        const auto orientation =
            static_cast< QskGradient::Orientation >( type - 2 );

        gradient.setOrientation( orientation );

        if ( inverted )
            gradient.reverse();

        setGradient( gradient );
    }
}

void Box::setBackground( FillType type, const QRgb base, bool inverted )
{
    if ( type == Unfilled )
    {
        setGradient( QskGradient() );
        return;
    }

    const QskHctColor htcColor( base );

    if ( type == Solid )
    {
        setGradient( htcColor.toned( 50 ).rgb() );
    }
    else
    {
        const auto dark = htcColor.toned( 40 ).rgb();
        const auto light = htcColor.toned( 70 ).rgb();

        const auto orientation =
            static_cast< QskGradient::Orientation >( type - 2 );

        if ( inverted )
            setGradient( orientation, dark, light );
        else
            setGradient( orientation, light, dark );
    }
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
    setGradient( QskGradient( QColor::fromRgba( rgb ) ) );
}

void Box::setGradient( Qt::GlobalColor color )
{
    setGradient( QskGradient( color ) );
}

void Box::setGradient( const QColor& color )
{
    setGradient( QskGradient( color ) );
}

void Box::setGradient( QskGradient::Orientation orientation,
    const QColor& color1, const QColor& color2 )
{
    setGradient( QskGradient( orientation, color1, color2 ) );
}

void Box::setGradient( QskGradient::Orientation orientation,
    const QColor& color1, const QColor& color2, const QColor& color3 )
{
    QskGradientStops stops;
    stops += QskGradientStop( 0.0, color1 );
    stops += QskGradientStop( 0.5, color2 );
    stops += QskGradientStop( 1.0, color3 );

    setGradient( QskGradient( orientation, stops ) );
}

void Box::setGradient( const QskGradient& gradient )
{
    setGradientHint( QskBox::Panel, gradient );
}

void Box::setGradient(
    const QskGradient::Orientation orientation, const QRgb base )
{
    const QskHctColor hctColor( base );

    QVector< QRgb > rgb;
    rgb.reserve( 10 );

    for ( int i = 0; i < 10; i++ )
        rgb += hctColor.toned( 90 - i * 7 ).rgb();

    setGradient( QskGradient( orientation, QskGradient::colorStops( rgb, true ) ) );
}
