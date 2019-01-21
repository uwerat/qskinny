/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_OBJECT_TREE_H
#define QSK_OBJECT_TREE_H 1

#include "QskControl.h"
#include "QskWindow.h"

#include <qvariant.h>

namespace QskObjectTree
{
    class Visitor
    {
      public:
        virtual ~Visitor() = default;

        virtual bool visitDown( QObject* object ) = 0;
        virtual bool visitUp( const QObject* object ) = 0;
    };

    QSK_EXPORT QObjectList childNodes( const QObject* );
    QSK_EXPORT QObject* parentNode( const QObject* );
    QSK_EXPORT bool isRoot( const QObject* );

    void traverseDown( QObject* object, Visitor& visitor );
    void traverseUp( QObject* object, Visitor& visitor );

    template< class T >
    class ResolveVisitor : public Visitor
    {
      public:
        ResolveVisitor( const char* propertyName )
            : m_propertyName( propertyName )
        {
        }

        inline const T& resolveValue() const
        {
            return m_value;
        }

        void setResolveValue( const T& value )
        {
            m_value = value;
        }

        bool visitDown( QObject* object ) override final
        {
            if ( auto control = qobject_cast< QskControl* >( object ) )
                return setImplicitValue( control, m_value );

            if ( auto window = qobject_cast< QskWindow* >( object ) )
                return setImplicitValue( window, m_value );

            return !setProperty( object, m_propertyName.constData(), m_value );
        }

        bool visitUp( const QObject* object ) override final
        {
            if ( isRoot( object ) )
                return true;

            if ( auto control = qobject_cast< const QskControl* >( object ) )
            {
                m_value = value( control );
                return true;
            }

            if ( auto window = qobject_cast< const QskWindow* >( object ) )
            {
                m_value = value( window );
                return true;
            }

            return getProperty( object, m_propertyName, m_value );
        }

      private:
        inline bool getProperty( const QObject* object,
            const char* name, T& value ) const
        {
            if ( !m_propertyName.isEmpty() )
            {
                const QVariant v = object->property( name );
                if ( v.canConvert< T >() )
                {
                    value = qvariant_cast< T >( v );
                    return true;
                }
            }

            return false;
        }

        inline bool setProperty( QObject* object,
            const char* name, const T& value ) const
        {
            T oldValue;
            if ( !getProperty( object, name, oldValue ) || oldValue == value )
                return false;

            object->setProperty( name, value );
            return true;
        }

        virtual bool setImplicitValue( QskControl*, const T& ) = 0;
        virtual bool setImplicitValue( QskWindow*, const T& ) = 0;

        virtual T value( const QskControl* ) const = 0;
        virtual T value( const QskWindow* ) const = 0;

      private:
        const QByteArray m_propertyName;
        T m_value;
    };
}

#endif
