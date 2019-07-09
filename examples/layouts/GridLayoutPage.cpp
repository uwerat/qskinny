/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridLayoutPage.h"

#include <QskRgbValue.h>
#include <QtQml>

static QQuickItem* qskCreateItemQml( const QUrl& url )
{
    QQmlEngine engine( nullptr );

    QQmlComponent component( &engine );
    component.loadUrl( url, QQmlComponent::PreferSynchronous );

    return qobject_cast< QQuickItem* >( component.create() );
}

GridLayoutPage::GridLayoutPage( QQuickItem* parent )
    : QskControl( parent )
{
    setMargins( 10 );
    setBackgroundColor( QskRgbValue::LightSteelBlue );

    setAutoLayoutChildren( true );

    if ( auto item = qskCreateItemQml( QUrl( "qrc:/qml/layouts.qml" ) ) )
    {
        item->setParentItem( this );
        item->setParent( this );
    }
}
