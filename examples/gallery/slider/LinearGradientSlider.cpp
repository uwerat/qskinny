#include "LinearGradientSlider.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>

LinearGradientSlider::LinearGradientSlider( QQuickItem* parent )
    : LinearGradientSlider( Qt::Horizontal, parent )
{
}

LinearGradientSlider::LinearGradientSlider( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( orientation, parent )
{
    const QVector< QskGradientStop > gradientStop = {
        { 0.0000, QColor::fromRgb( 255, 0, 0 ) },
        { 0.1667, QColor::fromRgb( 255, 255, 0 ) },
        { 0.3333, QColor::fromRgb( 0, 255, 0 ) },
        { 0.5000, QColor::fromRgb( 0, 255, 255 ) },
        { 0.6667, QColor::fromRgb( 0, 0, 255 ) },
        { 0.8333, QColor::fromRgb( 255, 0, 255 ) },
        { 1.0000, QColor::fromRgb( 255, 0, 0 ) },
    };

    const QskGradient gradient( orientation, gradientStop );
    setColor( Inherited::Fill, Qt::transparent );
    setGradientHint( Inherited::Groove, gradient );
    setBoxBorderColorsHint( Inherited::Handle, Qt::white );
    setBoxBorderMetricsHint( Inherited::Handle, 2 );

    connect( this, &QskSlider::valueChanged, this, [ this, gradient ]( qreal value ) {
        value = this->orientation() == Qt::Horizontal ? value : 1.0 - value;
        const auto selectedColor = gradient.extracted( value, value ).startColor();
        setColor( Inherited::Handle, selectedColor );
        setSelectedColor( selectedColor );
    } );
}

const QColor& LinearGradientSlider::selectedColor() const
{
    return m_selectedColor;
}

void LinearGradientSlider::setSelectedColor( const QColor& newSelectedColor )
{
    if ( m_selectedColor == newSelectedColor )
    {
        return;
    }
    m_selectedColor = newSelectedColor;
    Q_EMIT selectedColorChanged();
}
