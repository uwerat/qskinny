/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

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
    Q_OBJECT

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

Q_SIGNALS:
    void done( double, int );
};

class MyObject2: public MyObject
{
public:
    MyObject2( QObject* parent = nullptr ):
        MyObject( parent )
    {
    }

    virtual ~MyObject2()
    {
    }

    virtual void noop()
    {
    }
};

static auto fs = []( int i, double d ) { qDebug() << i << d; };

int main( int argc, char* argv[] )
{
    QCoreApplication app( argc, argv );

    MyObject object;
    MyObject2 object2;

    int num = 111;
    auto f = [&num]( int i, double d ) { qDebug() << i << d << (++num); };

    Invoker invoker;

#if 1
    invoker.addCallback( QskMetaFunction() );
    invoker.addCallback( debugValue );
    invoker.addCallback( debugValueI );
    invoker.addCallback( &object, &MyObject::print1 );
    invoker.addCallback( &object2, &MyObject2::print1 );
    invoker.addCallback( &object, &MyObject::print2 );
    invoker.addCallback( &object, &MyObject::print3 );
    invoker.addCallback( &object, &MyObject::print4 );
    invoker.addCallback( &object, []( double d, int i ) { qDebug() << d << i; } );

    invoker.addCallback( &object, f );
    invoker.addCallback( &object2, f );
    invoker.addCallback( &object, fs );
    invoker.addCallback( &object2, fs );

    invoker.addCallback( &object, []( double d ) { qDebug() << d; } );
    invoker.addCallback( []() { qDebug() << "HERE"; } );
    invoker.addCallback( []( int i, double d ) { qDebug() << i << d; } );
    invoker.addCallback( []( int i ) { qDebug() << i; } );
    invoker.addCallback( []( double d ) { qDebug() << d; } );
#endif

#if 1
    qDebug() << "== Direct Connections";
    invoker.invoke( 3.14, 35, Qt::DirectConnection );

    qDebug() << "\n\n== Queued Connections";

    QTimer::singleShot( 0,
        [&invoker] { invoker.invoke( 0.07, 42, Qt::QueuedConnection ); } );
#endif

    QTimer::singleShot( 100, &app, QCoreApplication::quit );

    return app.exec();
}

#include "main.moc"
