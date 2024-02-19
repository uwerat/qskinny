/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TestRectangle.h"
#include <QskFontRole.h>

TestRectangle::TestRectangle( QQuickItem* parent )
    : QskTextLabel( parent )
{
    setAlignment( Qt::AlignCenter );
    setFontRole( QskFontRole::Title );
    setTextColor( Qt::white );

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
