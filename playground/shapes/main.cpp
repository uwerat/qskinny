/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Window.h"
#include "GeometricShape.h"
#include "Stroke.h"

#include <QskQml.h>
#include <QskObjectCounter.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>

#include <QQmlApplicationEngine>

template< typename T >
static inline int registerType( const char* qmlName )
{
    return qmlRegisterType< T >( "Shapes", 1, 0, qmlName );
}

template< typename T >
static inline int registerValueType( const char* qmlName )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    QByteArray name = qmlName;
    name.data()[0] = std::tolower( name.data()[0] );
    return registerType< T >( name.constData() );
#else
    return qmlRegisterUncreatableType< T >( "Shapes", 1, 0, qmlName, QString() );
#endif
}

static bool doQml( int argc, char* argv[] )
{
    for ( int i = 1; i < argc; i++ )
    {
        if ( strcmp( argv[i], "-qml" ) == 0 )
            return true;
    }

    return false;
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    if ( doQml( argc, argv ) )
    {
        qDebug() << "Running QML";

        QskQml::registerTypes();

        registerType< GeometricShape >( "Shape" );
        registerValueType< Stroke >( "Stroke" );

        QQmlApplicationEngine engine( QUrl( "qrc:/qml/shapes.qml" ) );

        return app.exec();
    }
    else
    {
        qDebug() << "Running C++";

        Window window;
        window.show();

        return app.exec();
    }
}
