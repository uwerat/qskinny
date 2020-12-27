/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "OtherSlider.h"

#include <QskAspect.h>
#include <QskRgbValue.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>

#include <QskSkinHintTable.h>
#include <QskSkinHintTableEditor.h>

#include <cmath>

OtherSlider::OtherSlider( QQuickItem* parentItem )
    : QskSlider( parentItem )
{
    using A = QskAspect;
    using namespace QskRgb;

    const qreal h = 30;
    const qreal w = 2.0 * h;
    const qreal paddingW = 0.5 * w + 1;

    QskSkinHintTableEditor ed( &hintTable() );

    // Panel

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Panel | placement;

        ed.setMetric( aspect | A::Size, h );
        ed.setBoxShape( aspect, 4 );
        ed.setBoxBorderMetrics( aspect, 1 );
        ed.setBoxBorderColors( aspect, Grey900 );
        ed.setGradient( aspect, Grey400 );

        if ( placement == A::Horizontal )
            ed.setPadding( aspect, QskMargins( paddingW, 0 ) );
        else
            ed.setPadding( aspect, QskMargins( 0, paddingW ) );
    }

    // Groove

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Groove | placement;

        ed.setMetric( aspect | A::Size, 4 );
        ed.setBoxBorderMetrics( aspect, 0 );
        ed.setBoxShape( aspect, 1 );

        ed.setGradient( aspect, Qt::black );
    }

    // no Fill
    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Fill | placement;
        ed.setMetric( aspect | A::Size, 0 );
    }

    // Handle

    for ( auto placement : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Handle | placement;

        ed.setBoxBorderMetrics( aspect, 1 );
        ed.setBoxShape( aspect, 4 );

        const qreal m = 0.5 * std::ceil( 0.5 * ( w - h ) ) + 1;

        if ( placement == A::Horizontal )
            ed.setMargin( aspect, QskMargins( -m, 0 ) );
        else
            ed.setMargin( aspect, QskMargins( 0, -m ) );

        for ( auto state : { A::NoState, Pressed } )
        {
            ed.setBoxBorderColors( aspect | state, Grey600 );
            ed.setGradient( aspect | state, Blue400 );
        }
    }
}
