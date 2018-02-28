/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_INVOKABLE_H
#define QSK_META_INVOKABLE_H 1

#include "QskGlobal.h"
#include <QObject>

// helper classes for QskMetaFunction

#ifndef QT_NO_RTTI // we rely on hashing type_info

    /*
        When being enabled the same instance of QskMetaInvokable is used
        for all QskMetaFunctions having the same function/method/functor -
        f.e. &QQuickItem::update.

        Not sure, why QObject::connect does not do the same and always
        creates unique QSlotObjectBase objects for each connection.
     */
    #ifndef QSK_SHARED_META_INVOKABLE
        #define QSK_SHARED_META_INVOKABLE 1
    #endif
#endif

class QSK_EXPORT QskMetaInvokable : public QtPrivate::QSlotObjectBase
{
public:
    typedef void (* InvokeFunction)(
        int which, QtPrivate::QSlotObjectBase*, QObject*, void**, bool* );

    enum { TypeInfo = NumOperations + 1 };

    int typeInfo() const;
    int refCount() const;

protected:
    friend class QskMetaFunction;

    explicit QskMetaInvokable( InvokeFunction f ):
        QSlotObjectBase( f )
    {
    }

#if QSK_SHARED_META_INVOKABLE
    /*
        To avoid having more QskMetaInvokables for the same
        function we have a hash table, where they are registered
     */
    static QskMetaInvokable* find( const std::type_info& info );
    static void insert( const std::type_info&, QskMetaInvokable* );
    static void remove( const std::type_info& );
#else
    
#endif
};

template< typename Function, typename Args, typename R >
class QskMetaFunctionInvokable : public QskMetaInvokable
{
public:
    typedef QtPrivate::FunctionPointer< Function > FuncType;

    explicit QskMetaFunctionInvokable( Function function ):
        QskMetaInvokable( &invoke ),
        m_function( function )
    {
    }

    static void invoke(int which, QtPrivate::QSlotObjectBase* invokable,
        QObject* object, void** args, bool* )
    {
        auto invokableFunction = static_cast< QskMetaFunctionInvokable* >( invokable );

        switch ( which )
        {
            case Destroy:
            {
#if QSK_SHARE_INVOKABLES
                remove( typeid( Function ) );
#endif
                delete invokableFunction;

                break;
            }
            case Call:
            {
                FuncType::template call< Args, R >(
                    invokableFunction->m_function, object, args );
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

    Function m_function;
};

template< typename Function, typename Args, typename R >
class QskMetaMemberInvokable : public QskMetaInvokable
{
public:
    explicit QskMetaMemberInvokable( Function function ):
        QskMetaInvokable( &invoke ),
        m_function( function )
    {
    }

    static void invoke( int which, QtPrivate::QSlotObjectBase* invokable,
        QObject* object, void** args, bool* ret )
    {
        typedef QtPrivate::FunctionPointer< Function > FuncType;

        auto invokableMember = static_cast< QskMetaMemberInvokable* >( invokable );

        switch (which)
        {
            case Destroy:
            {
#if QSK_SHARE_INVOKABLES
                remove( typeid( Function ) );
#endif
                delete invokableMember;

                break;
            }
            case Call:
            {
                FuncType::template call< Args, R >(
                    invokableMember->m_function,
                    static_cast< typename FuncType::Object* >( object ), args );

                break;
            }
            case Compare:
            {
                const auto function = *reinterpret_cast< Function* >( args );
                *ret = function == invokableMember->m_function;
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
    Function m_function;
};

template< typename Function, int N, typename Args, typename R >
class QskMetaFunctorInvokable : public QskMetaInvokable
{
public:
    typedef QtPrivate::Functor< Function, N > FuncType;

    explicit QskMetaFunctorInvokable( Function function ):
        QskMetaInvokable( &invoke ),
        m_function( std::move( function ) )
    {
    }

    static void invoke( int which, QSlotObjectBase* invokable,
        QObject* object, void** args, bool* )
    {
        auto invokableFunctor = static_cast< QskMetaFunctorInvokable* >( invokable );

        switch (which)
        {
            case Destroy:
            {
#if QSK_SHARE_INVOKABLES
                remove( typeid( Function ) );
#endif
                delete invokableFunctor;

                break;
            }
            case Call:
            {
                FuncType::template call< Args, R >(
                    invokableFunctor->m_function, object, args );
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
    Function m_function;
};

#endif
