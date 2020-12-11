/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_FUNCTION_H
#define QSK_META_FUNCTION_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qobject.h>

namespace QskMetaFunctionTraits
{
    using namespace QtPrivate;

    template< typename T >
    using IsMemberFunction = typename std::enable_if<
        FunctionPointer< T >::IsPointerToMemberFunction, std::true_type >::type;

    template< typename T >
    using IsFunctorOrStaticFunction = typename std::enable_if<
        !FunctionPointer< T >::IsPointerToMemberFunction, std::true_type >::type;

    template< typename T >
    using IsFunctor = typename std::enable_if<
        !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount == -1, std::true_type >::type;

    template< typename T >
    using IsStaticFunction = typename std::enable_if<
        !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount >= 0, std::true_type >::type;

    template< typename T, IsMemberFunction< T >* = nullptr >
    constexpr inline int argumentCount()
    {
        using Traits = FunctionPointer< T >;
        return Traits::ArgumentCount;
    }

    template< typename T, IsStaticFunction< T >* = nullptr >
    constexpr inline int argumentCount()
    {
        using Traits = FunctionPointer< T >;
        return Traits::ArgumentCount;
    }

    template< typename T, IsFunctor< T >* = nullptr >
    constexpr inline int argumentCount()
    {
        using Traits = FunctionPointer< decltype( &T::operator() ) >;
        return Traits::ArgumentCount;
    }

    template< typename T, size_t i >
    constexpr typename std::enable_if< i >= argumentCount< T >(), int >::type argumentType()
    {
        return QMetaType::UnknownType;
    }

    template< typename T, size_t i, IsMemberFunction< T >* = nullptr >
    constexpr typename std::enable_if < i < argumentCount< T >(), int >::type argumentType()
    {
        return ConnectionTypes< typename FunctionPointer< T >::Arguments >::types()[ i ];
    }

    template< typename T, size_t i, IsStaticFunction< T >* = nullptr >
    constexpr typename std::enable_if < i < argumentCount< T >(), int >::type argumentType()
    {
        return ConnectionTypes< typename FunctionPointer< T >::Arguments >::types()[ i ];
    }

    template< typename T, size_t i, IsFunctor< T >* = nullptr >
    constexpr typename std::enable_if < i < argumentCount< T >(), int >::type argumentType()
    {
        using Traits = FunctionPointer< decltype( &T::operator() ) >;
        return ConnectionTypes< typename Traits::Arguments >::types()[ i ];
    }
}

class QSK_EXPORT QskMetaFunction
{
    Q_GADGET

  public:
    enum Type
    {
        Invalid = -1,

        // a non static method of class
        MemberFunction,

        // a static function, or static method of a class
        StaticFunction,

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

    template< typename T, QskMetaFunctionTraits::IsStaticFunction< T >* = nullptr >
    QskMetaFunction( T );

    ~QskMetaFunction();

    QskMetaFunction& operator=( const QskMetaFunction& );
    QskMetaFunction& operator=( QskMetaFunction&& );

    bool operator==( const QskMetaFunction& ) const;
    bool operator!=( const QskMetaFunction& ) const;

    explicit operator bool() const;

    int returnType() const;

    size_t parameterCount() const;
    const int* parameterTypes() const;

    void invoke( QObject*, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    Type functionType() const;
    bool isNull() const;

    class FunctionCall;
    FunctionCall* functionCall() const;

  protected:
    QskMetaFunction( FunctionCall* );

  private:
    FunctionCall* m_functionCall;
};

inline bool QskMetaFunction::operator!=( const QskMetaFunction& other ) const
{
    return !( *this == other );
}

inline QskMetaFunction::operator bool() const
{
    return !isNull();
}

inline bool QskMetaFunction::isNull() const
{
    return m_functionCall == nullptr;
}

inline QskMetaFunction::FunctionCall* QskMetaFunction::functionCall() const
{
    return m_functionCall;
}

#include "QskMetaFunction.hpp"

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

template< typename T, QskMetaFunctionTraits::IsStaticFunction< T >* >
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
