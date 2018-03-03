/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_FUNCTION_H
#define QSK_META_FUNCTION_H 1

#include "QskGlobal.h"
#include <QMetaType>

namespace QskMetaFunctionTraits
{
    using namespace QtPrivate;

    template< typename T >
    using IsMemberFunction = typename std::enable_if< FunctionPointer< T >::IsPointerToMemberFunction,
         std::true_type >::type;

    template< typename T >
    using IsFunctor = typename std::enable_if< !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount == -1, std::true_type >::type;

    template< typename T >
    using IsFunction = typename std::enable_if< !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount >= 0, std::true_type >::type;

    template< typename T >
    using IsFunction0 = typename std::enable_if< !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount == 0, std::true_type >::type;
}

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

    template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* = nullptr >
    QskMetaFunction( T );

    template< typename T, QskMetaFunctionTraits::IsFunctor< T >* = nullptr >
    QskMetaFunction( T );

    template< typename T, QskMetaFunctionTraits::IsFunction< T >* = nullptr >
    QskMetaFunction( T );

    ~QskMetaFunction();

    QskMetaFunction& operator=( const QskMetaFunction& );
    QskMetaFunction& operator=( QskMetaFunction&& );

    const int* parameterTypes() const;

    // including the return type !
    size_t parameterCount() const;

    void invoke( QObject*, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    Type functionType() const;

    class FunctionCall;
    FunctionCall* functionCall() const;

protected:
    QskMetaFunction( FunctionCall* );

private:
    FunctionCall* m_functionCall;
};

#include "QskMetaFunction.hpp"

inline QskMetaFunction::FunctionCall* QskMetaFunction::functionCall() const
{
    return m_functionCall;
}

inline const int* QskMetaFunction::parameterTypes() const
{
    return m_functionCall ? m_functionCall->parameterTypes() : nullptr;
}

template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* >
inline QskMetaFunction::QskMetaFunction( T function )
{
    using namespace QtPrivate;
    using namespace QskMetaFunctionCall;

    using Traits = FunctionPointer< T >;

    constexpr int Argc = Traits::ArgumentCount;
    using Args = typename List_Left< typename Traits::Arguments, Argc >::Value;

    m_functionCall = new MemberFunctionCall< T, Args, void >( function );
    m_functionCall->setParameterTypes(
        ConnectionTypes< typename Traits::Arguments >::types() );
}

template< typename T, QskMetaFunctionTraits::IsFunctor< T >* >
inline QskMetaFunction::QskMetaFunction( T functor )
{
    using namespace QtPrivate;
    using namespace QskMetaFunctionCall;

    using Traits = FunctionPointer< decltype( &T::operator() ) >;

    constexpr int Argc = Traits::ArgumentCount;
    using Args = typename List_Left< typename Traits::Arguments, Argc >::Value;

    m_functionCall = new FunctorFunctionCall< T, Argc, Args, void >( functor );
    m_functionCall->setParameterTypes(
        ConnectionTypes< typename Traits::Arguments >::types() );
}

template< typename T, QskMetaFunctionTraits::IsFunction< T >* >
inline QskMetaFunction::QskMetaFunction( T function )
{
    using namespace QtPrivate;
    using namespace QskMetaFunctionCall;

    using Traits = FunctionPointer< T >;

    constexpr int Argc = Traits::ArgumentCount;
    using Args = typename List_Left< typename Traits::Arguments, Argc >::Value;

    m_functionCall = new StaticFunctionCall< T, Args, void >( function );
    m_functionCall->setParameterTypes(
        ConnectionTypes< typename Traits::Arguments >::types() );
}

Q_DECLARE_METATYPE( QskMetaFunction )

#endif
