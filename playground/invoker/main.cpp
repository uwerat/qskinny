/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>

static void debugNone1()
{
    qDebug() << "None 1";
}

static void debugNone2()
{
    qDebug() << "None 2";
}

static void debugValueI1( int i )
{
    qDebug() << "I1" << i;
}

static void debugValueI2( int i )
{
    qDebug() << "I2" << i;
}

static void debugValueD( qreal d )
{
    qDebug() << "D" << d;
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

    void print0( double d, int i ) const
    {
        qDebug() << "print0" << d << i;
    }

    void print1( double d, int i ) const
    {
        qDebug() << "print1" << d << i;
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

static auto fs = []( int i, double d ) { qDebug() << i << d; };

class Application: public QCoreApplication
{
public:
    Application( int &argc, char *argv[] ):
        QCoreApplication( argc, argv ),
        m_object( new MyObject() ),
        m_thread( new QThread( this ) )
    {
        auto f = [this]( int i, double d ) { qDebug() << i << d << (++m_num); };

        m_invoker.addCallback( m_object, &MyObject::print0 );
        m_invoker.addCallback( m_object, &MyObject::print1 );
        m_invoker.addCallback( QskMetaFunction() );
        m_invoker.addCallback( debugNone1 );
        m_invoker.addCallback( debugNone2 );
        m_invoker.addCallback( debugValue );
        m_invoker.addCallback( debugValueI1 );
        m_invoker.addCallback( debugValueI2 );
        m_invoker.addCallback( debugValueD );
        m_invoker.addCallback( m_object, &MyObject::print0 );
        m_invoker.addCallback( m_object, &MyObject::print1 );
        m_invoker.addCallback( m_object, &MyObject::print2 );
        m_invoker.addCallback( m_object, &MyObject::print3 );
        m_invoker.addCallback( m_object, &MyObject::print4 );
        m_invoker.addCallback( m_object, []( double d, int i ) { qDebug() << d << i; } );
    
        m_invoker.addCallback( m_object, f );
        m_invoker.addCallback( m_object, fs );
    
        m_invoker.addCallback( m_object, []( double d ) { qDebug() << d; } );
        m_invoker.addCallback( []() { qDebug() << "HERE"; } );
        m_invoker.addCallback( []( int i, double d ) { qDebug() << i << d; } );
        m_invoker.addCallback( []( int i ) { qDebug() << "I1" << i; } );
        m_invoker.addCallback( []( int i ) { qDebug() << "I2" << i; } );
        m_invoker.addCallback( []( double d ) { qDebug() << "V" << d; } );
        m_invoker.addCallback( []( const double& d ) { qDebug() << "R" << d; } );

    }

    virtual ~Application()
    {
        delete m_object;
    }

    void invokeDirect()
    {
        qDebug() << "== Direct Connections";
        m_invoker.invoke( 3.14, 35, Qt::DirectConnection );
    }

    void invokeQueued()
    {
        qDebug() << "== Queued Connections";
        m_invoker.invoke( 0.07, 42, Qt::QueuedConnection );
    }

    void invokeBlockingQueued()
    {
        m_thread->start();
            
        m_object->moveToThread( m_thread );

        qDebug() << "== Blocking Queued Connections";
        m_invoker.invoke( 0.54, 88, Qt::BlockingQueuedConnection );

        QTimer::singleShot( 10, m_thread, &QThread::quit );
    }

private:
    Invoker m_invoker;
    MyObject* m_object;
    QThread* m_thread;

    int m_num = 111;
};

int main( int argc, char* argv[] )
{
    Application app( argc, argv );

    app.invokeDirect();

    QTimer::singleShot( 0, &app, &Application::invokeQueued );
    QTimer::singleShot( 20, &app, &Application::invokeBlockingQueued );

    QTimer::singleShot( 50, &app, QCoreApplication::quit );

    return app.exec();
}
