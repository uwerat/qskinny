/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"
#include <QDebug>

static void debugValueI( int i )
{
    qDebug() << i;
}

static void debugValue( qreal d, int i )
{
    qDebug() << d << i;
}

class MyObject : public QObject
{
public:
    MyObject( QObject* parent = nullptr ):
        QObject( parent )
    {
    }

    void print1( double d, int i ) const
    {
        qDebug() << d << i;
    }

    void print2( int i, double d ) const
    {
        qDebug() << i << d;
    }

    void print3( double d ) const
    {
        qDebug() << d;
    }

    void print4( int i ) const
    {
        qDebug() << i;
    }
};

int main( int argc, char* argv[] )
{
    Q_UNUSED( argc )
    Q_UNUSED( argv )

    MyObject object;

    Invoker invoker;

    invoker.addCallback( debugValue );
    invoker.addCallback( debugValueI );
    invoker.addCallback( &object, &MyObject::print1 );
    invoker.addCallback( &object, &MyObject::print2 );
    invoker.addCallback( &object, &MyObject::print3 );
    invoker.addCallback( &object, &MyObject::print4 );
    invoker.addCallback( &object, []( double d, int i ) { qDebug() << d << i; } );
    invoker.addCallback( &object, []( int i, double d ) { qDebug() << i << d; } );
    invoker.addCallback( &object, []( double d ) { qDebug() << d; } );
    invoker.addCallback( []() { qDebug() << "HERE"; } );
    invoker.addCallback( [ = ]( double d, int i ) { qDebug() << d << i; } );
    invoker.addCallback( []( int i, double d ) { qDebug() << i << d; } );
    invoker.addCallback( []( int i ) { qDebug() << i; } );
    invoker.addCallback( []( double d ) { qDebug() << d; } );

    invoker.invoke( 3.14, 35 );
}
