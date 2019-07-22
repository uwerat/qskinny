/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestRectangle.h"

TestRectangle::TestRectangle( QQuickItem* parent )
    : QskTextLabel( parent )
{
    setAlignment( Qt::AlignCenter );

    setPreferredSize( 10, 10 );
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Minimum );
    setFocusPolicy( Qt::TabFocus ); // for checking the focus tab chain
}

TestRectangle::TestRectangle( const char* colorName, QQuickItem* parent )
    : TestRectangle( parent )
{
    setColorName( colorName );
}

void TestRectangle::setColorName( const QString& colorName )
{
    m_colorName = colorName;

    setObjectName( colorName );
    setBackgroundColor( colorName );
}

QString TestRectangle::colorName() const
{
    return m_colorName;
}

#include "moc_TestRectangle.cpp"
