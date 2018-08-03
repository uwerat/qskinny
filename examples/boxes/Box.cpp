#include "Box.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>

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

void Box::setBackground( FillType type, Palette::Color color, bool inverted )
{
    if ( type == Unfilled )
    {
        setGradient( QskGradient() );
        return;
    }

    const auto pal = Palette::palette( color );

    const QColor light = pal.color( Palette::W300 );
    const QColor mid = pal.color( Palette::W600 );

    switch ( type )
    {
        case Unfilled:
            setGradient( QskGradient() );
            break;

        case Solid:
            setGradient( mid );
            break;

        default:
        {
            const auto orientation =
                static_cast< QskGradient::Orientation >( type - 2 );

            if ( inverted )
                setGradient( orientation, mid, light );
            else
                setGradient( orientation, light, mid );
        }
    }
}

void Box::setBorder( BorderType type, Palette::Color color )
{
    const auto pal = Palette::palette( color );

    setBorderWidth( 5 );

    QColor dark = pal.color( Palette::W700 );
    QColor mid = pal.color( Palette::W500 );
    QColor light = pal.color( Palette::W300 );
#if 0
    dark.setAlpha( 100 );
    mid.setAlpha( 100 );
    light.setAlpha( 100 );
#endif

    switch ( type )
    {
        case NoBorder:
            setBorderWidth( 0 );
            break;

        case Flat:
            setBorderColor( mid );
            break;

        case Raised1:
            setBorderColors( light, light, dark, dark );
            break;

        case Sunken1:
            setBorderColors( dark, dark, light, light );
            break;

        case Raised2:
            setBorderColors( mid, light, mid, dark );
            break;

        case Sunken2:
            setBorderColors( mid, dark, mid, light );
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

void Box::setBorderColors( const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom )
{
    QskBoxBorderColors colors( left, top, right, bottom );
    setBoxBorderColorsHint( QskBox::Panel, colors );
}

void Box::setBorderColor( const QColor& color )
{
    setBoxBorderColorsHint( QskBox::Panel, color );
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
    QVector< QskGradientStop > stops;
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
    const QskGradient::Orientation orientation, Palette::Color color )
{
    const auto pal = Palette::palette( color );

    QVector< QskGradientStop > stops;

    stops += QskGradientStop( 0.0, pal.color( static_cast< Palette::Weight >( 0 ) ) );

    const int count = Palette::NumWeights - 1;
    for ( int i = 1; i < count; i++ )
    {
        const qreal pos = qreal( i ) / count;
        const auto weight = static_cast< Palette::Weight >( i );

        stops += QskGradientStop( pos, stops.last().color() );
        stops += QskGradientStop( pos, pal.color( weight ) );
    }

    stops += QskGradientStop( 1.0,
        pal.color( static_cast<Palette::Weight>( Palette::NumWeights - 1 ) ) );

    setGradient( QskGradient( orientation, stops ) );
}
