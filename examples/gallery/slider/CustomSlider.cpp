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
    using namespace QskRgb;

    setBoxShapeHint( Fill, 0 );
    setGradientHint( Fill, Grey700 );
    setColor( Scale, qRgb( 178, 178, 178 ) ); // for the ticks

    setMetric( Handle | QskAspect::Size, 80 );
    setColor( Handle, Grey800 );

    for ( auto state : { Pressed, Focused | Hovered, Hovered, Focused } )
        setColor( Handle | state, Orange600 );

    setAnimation( QskSlider::Handle | QskAspect::Color, 1000 );
    for ( auto state : { Focused | Hovered, Hovered, Focused } )
        setAnimation( QskSlider::Handle | QskAspect::Color | state, 300 );

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
    return subControlRect( Handle );
}
