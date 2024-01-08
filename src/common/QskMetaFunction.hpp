/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_META_FUNCTION_HPP
#define QSK_META_FUNCTION_HPP

#include "QskGlobal.h"
#include <qobject.h>

class QskMetaFunction::FunctionCall : public QtPrivate::QSlotObjectBase
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
    explicit inline FunctionCall( InvokeFunction f,
            const int* parameterTypes = nullptr ):
        QSlotObjectBase( f ),
        m_parameterTypes( parameterTypes )
    {
    }

  private:
    const int* m_parameterTypes; // static array, only needed for Qt::QueuedConnection
};

namespace QskMetaFunctionCall
{
    template< typename Function, typename Args, typename R >
    class StaticFunctionCall : public QskMetaFunction::FunctionCall
    {
        using MetaCall = StaticFunctionCall< Function, Args, R >;

      public:
        explicit inline StaticFunctionCall( Function function ):
            FunctionCall( &invoke ),
            m_function( function )
        {
        }

        static void invoke(int which, QSlotObjectBase* functionCall,
            QObject* object, void** args, bool* ret )
        {
            switch ( which )
            {
                case Destroy:
                {
                    delete static_cast< MetaCall* >( functionCall );
                    break;
                }
                case Call:
                {
                    using FuncType = QtPrivate::FunctionPointer< Function >;

                    FuncType::template call< Args, R >(
                        static_cast< MetaCall* >( functionCall )->m_function, object, args );
                    break;
                }
                case Compare:
                {
                    *ret = reinterpret_cast< MetaCall* >( args )->m_function
                        == static_cast< MetaCall* >( functionCall )->m_function;

                    break;
                }
                case TypeInfo:
                {
                    *reinterpret_cast< int* >( args ) = QskMetaFunction::StaticFunction;
                    break;
                }
            }
        }

      private:
        Function m_function;
    };

    template< typename Function, typename Args, typename R >
    class MemberFunctionCall : public QskMetaFunction::FunctionCall
    {
        using MetaCall = MemberFunctionCall< Function, Args, R >;

      public:
        explicit inline MemberFunctionCall( Function function ):
            FunctionCall( &invoke ),
            m_function( function )
        {
        }

        static void invoke( int which, QSlotObjectBase* functionCall,
            QObject* object, void** args, bool* )
        {
            switch (which)
            {
                case Destroy:
                {
                    delete static_cast< MetaCall* >( functionCall );
                    break;
                }
                case Call:
                {
                    using FuncType = QtPrivate::FunctionPointer< Function >;

                    FuncType::template call< Args, R >(
                        static_cast< MetaCall* >( functionCall )->m_function,
                        static_cast< typename FuncType::Object* >( object ), args );

                    break;
                }
                case TypeInfo:
                {
                    *reinterpret_cast< int* >( args ) = QskMetaFunction::MemberFunction;
                    break;
                }
            }
        }

      private:
        Function m_function;
    };

    template< typename Function, int N, typename Args, typename R >
    class FunctorFunctionCall : public QskMetaFunction::FunctionCall
    {
        using MetaCall = FunctorFunctionCall< Function, N, Args, R >;

      public:
        explicit inline FunctorFunctionCall( Function function ):
            FunctionCall( &invoke ),
            m_function( function )
        {
        }

        static void invoke( int which, QSlotObjectBase* functionCall,
            QObject* object, void** args, bool* )
        {
            switch (which)
            {
                case Destroy:
                {
                    delete static_cast< MetaCall* >( functionCall );
                    break;
                }
                case Call:
                {
#if QT_VERSION < QT_VERSION_CHECK( 6, 7, 0 )
                    using FuncType = QtPrivate::Functor< Function, N >;
#else
                    using FuncType = QtPrivate::Callable< Function, Args >;
#endif

                    FuncType::template call< Args, R >(
                        static_cast< MetaCall* >( functionCall )->m_function, object, args );

                    break;
                }
                case TypeInfo:
                {
                    *reinterpret_cast< int* >( args ) = QskMetaFunction::Functor;
                    break;
                }
            }
        }

      private:
        Function m_function;
    };
}

#endif
