/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

void debugNone1()
{
    qDebug() << "None 1";
}

void debugNone2()
{
    qDebug() << "None 2";
}

void debugValueI1( int i )
{
    qDebug() << "I1" << i;
}

void debugValueI2( int i )
{
    qDebug() << "I2" << i;
}

void debugValueD( qreal d )
{
    qDebug() << "D" << d;
}

void debugValue( qreal d, int i )
{
    qDebug() << d << i;
}

class MyObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY( int number WRITE setNumber )
    Q_PROPERTY( qreal value WRITE setValue )
    Q_PROPERTY( QString valueString WRITE setValueString )

  public:
    MyObject( QObject* parent = nullptr )
        : QObject( parent )
    {
    }

    Q_INVOKABLE void printInvokable( double d, int i ) const
    {
        qDebug() << "invokable" << d << i;
    }

    void setNumber( int number )
    {
        qDebug() << "setNumber" << number;
    }

    void setValue( qreal value )
    {
        qDebug() << "setValue" << value;
    }

    void setValueString( const QString& s )
    {
        qDebug() << "setValueString" << s;
    }

  public Q_SLOTS:
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

    void printS( const QString& s ) const
    {
        qDebug() << s;
    }
};

static auto fs = []( int i, double d ) { qDebug() << i << d; };

class Application : public QCoreApplication
{
  public:
    Application( int& argc, char* argv[] )
        : QCoreApplication( argc, argv )
        , m_object( new MyObject() )
        , m_thread( new QThread( this ) )
    {
#if 1
        m_invoker.addPropertyCall( m_object, "number" );
        m_invoker.addPropertyCall( m_object, "value" );
        m_invoker.addPropertyCall( m_object, "valueString" );
#endif

#if 1
        m_invoker.addMethodCall( m_object, "print0(double,int)" );
        m_invoker.addMethodCall( m_object, "print1(double,int)" );
        m_invoker.addMethodCall( m_object, SLOT(print2(int,double)) );
        m_invoker.addMethodCall( m_object, "print3(double)" );
        m_invoker.addMethodCall( m_object, "print4(int)" );
        m_invoker.addMethodCall( m_object, "print4(int)" );
        m_invoker.addMethodCall( m_object, "printS(QString)" );
        m_invoker.addMethodCall( m_object, "printInvokable(double,int)" );
#endif

#if 1
        auto f = [this]( int i, double d ) { qDebug() << i << d << ( ++m_num ); };

        m_invoker.addFunctionCall( m_object, &MyObject::print0 );
        m_invoker.addFunctionCall( m_object, &MyObject::print1 );
        m_invoker.addFunctionCall( QskMetaFunction() );
        m_invoker.addFunctionCall( debugNone1 );
        m_invoker.addFunctionCall( debugNone2 );
        m_invoker.addFunctionCall( debugValue );
        m_invoker.addFunctionCall( debugValueI1 );
        m_invoker.addFunctionCall( debugValueI2 );
        m_invoker.addFunctionCall( debugValueD );
        m_invoker.addFunctionCall( m_object, &MyObject::print0 );
        m_invoker.addFunctionCall( m_object, &MyObject::print1 );
        m_invoker.addFunctionCall( m_object, &MyObject::print2 );
        m_invoker.addFunctionCall( m_object, &MyObject::print3 );
        m_invoker.addFunctionCall( m_object, &MyObject::print4 );
        m_invoker.addFunctionCall( m_object, &MyObject::printS );
        m_invoker.addFunctionCall( m_object, []( double d, int i ) { qDebug() << d << i; } );

        m_invoker.addFunctionCall( m_object, f );
        m_invoker.addFunctionCall( m_object, fs );

        m_invoker.addFunctionCall( m_object, []( double d ) { qDebug() << d; } );
        m_invoker.addFunctionCall( []() { qDebug() << "HERE"; } );
        m_invoker.addFunctionCall( []( int i, double d ) { qDebug() << i << d; } );
        m_invoker.addFunctionCall( []( int i ) { qDebug() << "I1" << i; } );
        m_invoker.addFunctionCall( []( int i ) { qDebug() << "I2" << i; } );
        m_invoker.addFunctionCall( []( double d ) { qDebug() << "V" << d; } );
        m_invoker.addFunctionCall( []( const double& d ) { qDebug() << "R" << d; } );
#endif
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

#include "main.moc"
