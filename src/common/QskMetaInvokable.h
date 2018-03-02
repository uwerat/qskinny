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
        TypeInfo = NumOperations + 1
    };

    int typeInfo() const;
    int refCount() const;

    inline const int* parameterTypes() const
    {
        return m_parameterTypes;
    }

    inline void setParameterTypes( const int* types )
    {
        m_parameterTypes = types;
    }

protected:
    explicit inline QskMetaInvokable( InvokeFunction f,
            const int* m_parameterTypes = nullptr ):
        QSlotObjectBase( f ),
        m_parameterTypes( m_parameterTypes )
    {
    }

private:
    const int* m_parameterTypes; // static array, only needed for Qt::QueuedConnection
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

    static void invoke(int which, QtPrivate::QSlotObjectBase* invokable,
        QObject* object, void** args, bool* )
    {
        switch ( which )
        {
            case Destroy:
            {
                delete static_cast< Invokable* >( invokable );
                break;
            }
            case Call:
            {
                typedef QtPrivate::FunctionPointer< Function > FuncType;

                FuncType::template call< Args, R >(
                    static_cast< Invokable* >( invokable )->m_function, object, args );
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

    static void invoke( int which, QtPrivate::QSlotObjectBase* slotObject,
        QObject* object, void** args, bool* )
    {
        switch (which)
        {
            case Destroy:
            {
                delete static_cast< Invokable* >( slotObject );
                break;
            }
            case Call:
            {
                typedef QtPrivate::FunctionPointer< Function > FuncType;

                FuncType::template call< Args, R >(
                    static_cast< Invokable* >( slotObject )->m_function,
                    static_cast< typename FuncType::Object* >( object ), args );

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

    static void invoke( int which, QSlotObjectBase* slotObject,
        QObject* object, void** args, bool* )
    {
        switch (which)
        {
            case Destroy:
            {
                delete static_cast< Invokable* >( slotObject );
                break;
            }
            case Call:
            {
                typedef QtPrivate::Functor< Function, N > FuncType;

                FuncType::template call< Args, R >(
                    static_cast< Invokable* >( slotObject )->m_function, object, args );

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
