/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_INVOKABLE_H
#define QSK_META_INVOKABLE_H 1

#include "QskGlobal.h"
#include <QObject>

// helper classes for QskMetaFunction

class QSK_EXPORT QskMetaInvokable : public QtPrivate::QSlotObjectBase
{
public:
    typedef void (* InvokeFunction)(
        int which, QtPrivate::QSlotObjectBase*, QObject*, void**, bool* );

    enum
    {
        TypeInfo = NumOperations + 1,
        Create,
        Find
    };

    int typeInfo() const;
    int refCount() const;

    static QskMetaInvokable* instance( InvokeFunction, void** function );

protected:
    explicit inline QskMetaInvokable( InvokeFunction f ):
        QSlotObjectBase( f )
    {
    }
};

template< typename Function, typename Args, typename R >
class QskMetaFunctionInvokable : public QskMetaInvokable
{
    using Invokable = QskMetaFunctionInvokable< Function, Args, R >;

public:
    explicit inline QskMetaFunctionInvokable( Function function ):
        QskMetaInvokable( &invoke ),
        m_function( function )
    {
    }

    static void invoke(int which, QtPrivate::QSlotObjectBase*,
        QObject* object, void** args, bool* )
    {
        static Invokable* invokable = nullptr;

        switch ( which )
        {
            case Find:
            {
                *reinterpret_cast< void** >( args[0] ) = invokable;
                break;
            }
            case Create:
            {
                invokable = new Invokable( *reinterpret_cast< Function* >( args[1] ) );
                *reinterpret_cast< void** >( args[0] ) = invokable;

                break;
            }
            case Destroy:
            {
                delete invokable;
                invokable = nullptr;

                break;
            }
            case Call:
            {
                typedef QtPrivate::FunctionPointer< Function > FuncType;

                FuncType::template call< Args, R >(
                    invokable->m_function, object, args );
                break;
            }
            case TypeInfo:
            {
                *reinterpret_cast< int* >( args ) = 1; // QskMetaFunction::Function
                break;
            }
        }
    }

private:
    Function m_function;
};

template< typename Function, typename Args, typename R >
class QskMetaMemberInvokable : public QskMetaInvokable
{
    using Invokable = QskMetaMemberInvokable< Function, Args, R >;

public:
    explicit inline QskMetaMemberInvokable( Function function ):
        QskMetaInvokable( &invoke ),
        m_function( function )
    {
    }

    static void invoke( int which, QtPrivate::QSlotObjectBase*,
        QObject* object, void** args, bool* ret )
    {
        static Invokable* invokable = nullptr;

        switch (which)
        {
            case Find:
            {
                *reinterpret_cast< void** >( args[0] ) = invokable;
                break;
            }
            case Create:
            {
                invokable = new Invokable( *reinterpret_cast< Function* >( args[1] ) );
                *reinterpret_cast< void** >( args[0] ) = invokable;

                break;
            }
            case Destroy:
            {
                delete invokable;
                invokable = nullptr;

                break;
            }
            case Call:
            {
                typedef QtPrivate::FunctionPointer< Function > FuncType;

                FuncType::template call< Args, R >(
                    invokable->m_function,
                    static_cast< typename FuncType::Object* >( object ), args );

                break;
            }
            case Compare:
            {
                const auto function = *reinterpret_cast< Function* >( args );
                *ret = function == invokable->m_function;
                break;
            }
            case TypeInfo:
            {
                *reinterpret_cast< int* >( args ) = 0; // = QskMetaFunction::Member
                break;
            }
        }
    }

private:
    Function m_function;
};

template< typename Function, int N, typename Args, typename R >
class QskMetaFunctorInvokable : public QskMetaInvokable
{
    using Invokable = QskMetaFunctorInvokable< Function, N, Args, R >;

public:
    explicit inline QskMetaFunctorInvokable( Function function ):
        QskMetaInvokable( &invoke ),
        m_function( function )
    {
    }

    static void invoke( int which, QSlotObjectBase*, QObject* object, void** args, bool* )
    {
        static Invokable* invokable = nullptr;

        switch (which)
        {
            case Find:
            {
                *reinterpret_cast< void** >( args[0] ) = invokable;
                break;
            }
            case Create:
            {
                invokable = new Invokable( *reinterpret_cast< Function* >( args[1] ) );
                *reinterpret_cast< void** >( args[0] ) = invokable;

                break;
            }
            case Destroy:
            {
                delete invokable;
                invokable = nullptr;

                break;
            }
            case Call:
            {
                typedef QtPrivate::Functor< Function, N > FuncType;

                FuncType::template call< Args, R >(
                    invokable->m_function, object, args );

                break;
            }
            case TypeInfo:
            {
                *reinterpret_cast< int* >( args ) = 2; // QskMetaFunction::Functor
                break;
            }
        }
    }

private:
    Function m_function;
};

#endif
