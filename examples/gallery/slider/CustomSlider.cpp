/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "CustomSlider.h"
#include "CustomSliderSkinlet.h"

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskRgbValue.h>

QSK_SUBCONTROL( CustomSlider, Scale )
QSK_SUBCONTROL( CustomSlider, Decoration )

CustomSlider::CustomSlider( QQuickItem* parentItem )
    : QskSlider( parentItem )
{
    using namespace QskAspect;

    setMetric( QskSlider::Handle | Size, 80 );

#if 0
    const QskGradient fillGradient( QskGradient::Horizontal,
        QskRgb::Grey700, QskRgb::Grey500 );
#else
    const QskGradient fillGradient( QskRgb::Grey700 );
#endif

    setBoxShapeHint( Fill, 0 );
    setGradientHint( Fill, fillGradient );
    setColor( Scale, qRgb( 178, 178, 178 ) ); // for the ticks

    setColor( QskSlider::Handle, QskRgb::Grey800 );

    for ( auto state : { Pressed, Focused | Hovered, Hovered, Focused } )
        setColor( QskSlider::Handle | Color | state, QskRgb::Orange600 );

    setAnimation( QskSlider::Handle | Color, 1000 );
    for ( auto state : { Focused | Hovered, Hovered, Focused } )
        setAnimation( QskSlider::Handle | Color | state, 300 );

    // using an individual skinlet, not known by the skin

    auto skinlet = new CustomSliderSkinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );

    connect( this, &QskSlider::valueChanged,
        this, &QskControl::focusIndicatorRectChanged );
}

QSizeF CustomSlider::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    auto size = Inherited::contentsSizeHint( which, constraint );

    if ( which == Qt::PreferredSize && size.height() >= 0 )
        size.setHeight( size.height() + 40 );

    return size;
}

QRectF CustomSlider::focusIndicatorRect() const
{
    return subControlRect( QskSlider::Handle );
}
