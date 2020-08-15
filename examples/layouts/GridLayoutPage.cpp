/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridLayoutPage.h"
#include <QskRgbValue.h>

#define USE_QML 1

#if USE_QML

#include <QtQml>

static void insertItemQml( QQuickItem* parentItem, const QUrl& url )
{
    QQmlEngine engine( nullptr );

    QQmlComponent component( &engine );
    component.loadUrl( url, QQmlComponent::PreferSynchronous );

    if( auto item = qobject_cast< QQuickItem* >( component.create() ) )
    {
        item->setParentItem( parentItem );
        item->setParent( parentItem );
    }
}

#else

#include <QskGridBox.h>
#include "TestRectangle.h"

namespace
{
    class Box : public QskGridBox
    {
      public:
        Box( QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setObjectName( "GridBox" );

            setBackgroundColor( Qt::white );
            setMargins( 10 );

            addItem( new TestRectangle( "PaleVioletRed" ), 0, 0, 1, 2 );
            addItem( new TestRectangle( "DarkSeaGreen" ), 1, 0, 2, 1 );
            addItem( new TestRectangle( "SkyBlue" ), 2, 1, 1, 1 );
            addItem( new TestRectangle( "NavajoWhite" ), 0, 2, -1, 1 );

            setRowStretchFactor( 0, 1 );
            setRowStretchFactor( 1, 2 );
            setRowStretchFactor( 2, 1 );
        }

        void mirror()
        {
            setLayoutMirroring( !layoutMirroring() );
        }
    };
}

#endif

GridLayoutPage::GridLayoutPage( QQuickItem* parent )
    : QskControl( parent )
{
    setMargins( 10 );
    setBackgroundColor( QskRgb::LightSteelBlue );

    setAutoLayoutChildren( true );

#if USE_QML
    insertItemQml( this, QUrl( "qrc:/qml/layouts.qml" ) );
#else
    new Box( this );
#endif
}
