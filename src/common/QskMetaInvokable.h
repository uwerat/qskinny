/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_INVOKABLE_H
#define QSK_META_INVOKABLE_H 1

#include "QskGlobal.h"
#include <QObject>

// helper classes for QskMetaFunction

class QskMetaInvokable : public QtPrivate::QSlotObjectBase
{
public:
    typedef void (* InvokeFunction)(
        int which, QtPrivate::QSlotObjectBase*, QObject*, void**, bool* );

    enum { TypeInfo = NumOperations + 1 };

    int typeInfo() const;
    int refCount() const;

protected:
    explicit QskMetaInvokable( InvokeFunction f ):
        QSlotObjectBase( f )
    {
    }
};

template< typename Func, typename Args, typename R >
class QskMetaFunctionInvokable : public QskMetaInvokable
{
public:
    typedef QtPrivate::FunctionPointer< Func > FuncType;

    explicit QskMetaFunctionInvokable( Func f ):
        QskMetaInvokable( &invoke ),
        function(f)
    {
    }

    static void invoke(int which, QtPrivate::QSlotObjectBase* invokable,
        QObject* object, void** args, bool* )
    {
        auto f = static_cast< QskMetaFunctionInvokable* >( invokable );

        switch ( which )
        {
            case Destroy:
            {
                delete f;
                break;
            }
            case Call:
            {
                FuncType::template call< Args, R >( f->function, object, args );
                break;
            }
            case TypeInfo:
            {
                int* typeInfo = reinterpret_cast< int* >( args );
                *typeInfo = 1; // = QskMetaFunction::Function

                break;
            }
        }
    }

    Func function;
};

template< typename Func, typename Args, typename R >
class QskMetaMemberInvokable : public QskMetaInvokable
{
public:
    explicit QskMetaMemberInvokable( Func f ):
        QskMetaInvokable( &invoke ),
        function(f)
    {
    }

    static void invoke( int which, QtPrivate::QSlotObjectBase* invokable,
        QObject* object, void** args, bool* ret )
    {
        typedef QtPrivate::FunctionPointer< Func > FuncType;

        auto f = static_cast< QskMetaMemberInvokable* >( invokable );

        switch (which)
        {
            case Destroy:
            {
                delete f;
                break;
            }
            case Call:
            {
                FuncType::template call< Args, R >(
                    f->function, static_cast< typename FuncType::Object* >( object ), args );

                break;
            }
            case Compare:
            {
                *ret = *reinterpret_cast< Func* >( args ) == f->function;
                break;
            }
            case TypeInfo:
            {
                int* typeInfo = reinterpret_cast< int* >( args );
                *typeInfo = 0; // = QskMetaFunction::Member

                break;
            }
        }
    }

private:
    Func function;
};

template< typename Func, int N, typename Args, typename R >
class QskMetaFunctorInvokable : public QskMetaInvokable
{
public:
    typedef QtPrivate::Functor< Func, N > FuncType;

    explicit QskMetaFunctorInvokable( Func f ):
        QskMetaInvokable( &invoke ),
        function( std::move( f ) )
    {
    }

    static void invoke( int which, QSlotObjectBase* invokable,
        QObject* object, void** args, bool* )
    {
        auto f = static_cast< QskMetaFunctorInvokable* >( invokable );

        switch (which)
        {
            case Destroy:
            {
                delete f;
                break;
            }
            case Call:
            {
                FuncType::template call< Args, R >( f->function, object, args );
                break;
            }
            case TypeInfo:
            {
                int* typeInfo = reinterpret_cast< int* >( args );
                *typeInfo = 2; // QskMetaFunction::Functor;

                break;
            }
        }
    }

private:
    Func function;
};

#endif
