/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
            const int* m_parameterTypes = nullptr ):
        QSlotObjectBase( f ),
        m_parameterTypes( m_parameterTypes )
    {
    }

  private:
    const int* m_parameterTypes; // static array, only needed for Qt::QueuedConnection
};

namespace QskMetaFunctionCall
{
    using FunctionCall = QskMetaFunction::FunctionCall;
    using namespace QtPrivate;

    template< typename Function, typename Args, typename R >
    class StaticFunctionCall : public FunctionCall
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
                    typedef FunctionPointer< Function > FuncType;

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
    class MemberFunctionCall : public FunctionCall
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
                    typedef FunctionPointer< Function > FuncType;

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
    class FunctorFunctionCall : public FunctionCall
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
                    typedef Functor< Function, N > FuncType;

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
