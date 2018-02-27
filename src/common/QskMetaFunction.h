/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_FUNCTION_H
#define QSK_META_FUNCTION_H 1

#include "QskGlobal.h"
#include "QskMetaCall.h"

#include <QMetaType>

class QSK_EXPORT QskMetaFunction
{
    Q_GADGET

public:
    enum Type
    {
        Invalid = -1,

        // a non static method of class
        Member,

        // a static function, or static method of a class
        Function,

        // a functor or lambda
        Functor
    };

    Q_ENUM( Type )

    QskMetaFunction();

    QskMetaFunction( const QskMetaFunction& );
    QskMetaFunction( QskMetaFunction&& );

    template< typename T, QskMetaCall::IsMemberFunction< T >* = nullptr >
    QskMetaFunction( T );

    template< typename T, QskMetaCall::IsFunction< T >* = nullptr >
    QskMetaFunction( T );

    template< typename T, QskMetaCall::IsFunctor< T >* = nullptr >
    QskMetaFunction( T );

    ~QskMetaFunction();

    QskMetaFunction& operator=( const QskMetaFunction& );
    QskMetaFunction& operator=( QskMetaFunction&& );

    const int* parameterTypes() const;

    void invoke( QObject*, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    Type functionType() const;

private:
    QskMetaCall::Invokable* m_invokable;
    const int* m_parameterTypes;
};


template< typename T, QskMetaCall::IsMemberFunction< T >* >
inline QskMetaFunction::QskMetaFunction( T function )
{
    using namespace QtPrivate;
    using namespace QskMetaCall;

    using Traits = FunctionPointer< T >;

    const int Argc = Traits::ArgumentCount;
    using Args = typename List_Left< typename Traits::Arguments, Argc >::Value;

    m_invokable = new MemberFunctionInvokable< T, Args, void >( function );
    m_parameterTypes = ConnectionTypes< typename Traits::Arguments >::types();
}

template< typename T, QskMetaCall::IsFunction< T >* >
inline QskMetaFunction::QskMetaFunction( T function )
{
    using namespace QtPrivate;
    using namespace QskMetaCall;

    using Traits = FunctionPointer< T >;

    const int Argc = Traits::ArgumentCount;
    using Args = typename List_Left< typename Traits::Arguments, Argc >::Value;

    m_invokable = new FunctionInvokable< T, Args, void >( function );
    m_parameterTypes = ConnectionTypes< typename Traits::Arguments >::types();
}

template< typename T, QskMetaCall::IsFunctor< T >* >
inline QskMetaFunction::QskMetaFunction( T functor )
{
    using namespace QtPrivate;

    using Traits = FunctionPointer< decltype( &T::operator() ) >;

    const int Argc = Traits::ArgumentCount;
    using Args = typename List_Left< typename Traits::Arguments, Argc >::Value;

    m_invokable = new QskMetaCall::FunctorInvokable< T, Argc, Args, void > ( functor );
    m_parameterTypes = ConnectionTypes< typename Traits::Arguments >::types();
}

Q_DECLARE_METATYPE( QskMetaFunction )

#endif
