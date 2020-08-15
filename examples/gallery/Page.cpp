/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Page.h"

#include <QskRgbValue.h>
#include <QskBoxShapeMetrics.h>

Page::Page( Qt::Orientation orientation, QQuickItem* parent )
    : QskLinearBox( orientation, parent )
{
    setBackgroundColor( Qt::gray );

    setPanel( true );
    setBoxShapeHint( QskBox::Panel, 8 );
    setGradient( QskRgb::GhostWhite );

    setMargins( 5 );
    setPadding( 10 );
    setSpacing( 10 );
}

void Page::setGradient( const QskGradient& gradient )
{
    setGradientHint( QskBox::Panel, gradient );
}
