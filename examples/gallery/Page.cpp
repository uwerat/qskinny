/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Page.h"
#include <QskRgbValue.h>

Page::Page( Qt::Orientation orientation, QQuickItem* parent )
    : QskLinearBox( orientation, parent )
    , m_gradient( QskRgb::GhostWhite )
{
    setMargins( 20 );
    setPadding( 10 );
    setSpacing( 10 );
}

void Page::setGradient( const QskGradient& gradient )
{
    if ( gradient != m_gradient )
    {
        m_gradient = gradient;

        if ( parentItem() && isVisibleToParent() )
            parentItem()->update();
    }
}

QskGradient Page::gradient() const
{
    return m_gradient;
}
