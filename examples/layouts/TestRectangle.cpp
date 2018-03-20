/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestRectangle.h"

#include <QskAspect.h>

TestRectangle::TestRectangle( const char* colorName, QQuickItem* parent ):
    QskTextLabel( parent )
{
    setObjectName( colorName );

    setAlignment( Qt::AlignCenter );
    setBackgroundColor( colorName );

    setPreferredSize( 10, 10 );
    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Minimum );
    setFocusPolicy( Qt::TabFocus ); // for checking the focus tab chain
}
