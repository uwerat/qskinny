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

#include <QskSkinHintTable.h>
#include <QskSkinHintTableEditor.h>

QSK_SUBCONTROL( CustomSlider, Scale )
QSK_SUBCONTROL( CustomSlider, Decoration )

CustomSlider::CustomSlider( QQuickItem* parentItem )
    : QskSlider( parentItem )
{
    using namespace QskRgb;

    QskSkinHintTableEditor ed( &hintTable() );

    ed.setBoxShape( Fill, 0 );
    ed.setGradient( Fill, Grey700 );
    ed.setColor( Scale, qRgb( 178, 178, 178 ) ); // for the ticks

    ed.setStrutSize( Handle, 80, 80 );
    ed.setColor( Handle, Grey800 );

    for ( auto state : { Pressed, Focused | Hovered, Hovered, Focused } )
        ed.setColor( Handle | state, Orange600 );

    ed.setAnimation( Handle | QskAspect::Color, 1000 );
    for ( auto state : { Focused | Hovered, Hovered, Focused } )
        ed.setAnimation( Handle | QskAspect::Color | state, 300 );

    // using an individual skinlet, not known by the skin

    auto skinlet = new CustomSliderSkinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );

    connect( this, &QskSlider::valueChanged,
        this, &QskControl::focusIndicatorRectChanged );
}

QRectF CustomSlider::focusIndicatorRect() const
{
    return subControlRect( Handle );
}
