/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_INVOKABLE_H
#define QSK_META_INVOKABLE_H 1

#include "QskGlobal.h"

#include <QMetaType>
#include <Qt>

template< typename T > class QVector;

class QskMetaFunction;
class QMetaObject;
class QMetaMethod;
class QMetaProperty;
class QObject;

class QSK_EXPORT QskMetaInvokable
{
public:
    enum Type
    {
        Invalid = 0,

        // A QMetaMethod
        MetaMethod,

        // The WRITE accessor of a QMetaProperty
        MetaProperty,

        // A function pointer, for what Qt calls "functor based" callbacks
        MetaFunction
    };

    QskMetaInvokable();

    QskMetaInvokable( const QskMetaFunction& );

    QskMetaInvokable( const QMetaMethod& );
    QskMetaInvokable( const QObject*, const char* methodName );
    QskMetaInvokable( const QMetaObject*, const char* methodName );

    QskMetaInvokable( const QMetaProperty& );

    QskMetaInvokable( const QskMetaInvokable& );

    ~QskMetaInvokable();

    QskMetaInvokable& operator=( const QskMetaInvokable& );

    bool operator==( const QskMetaInvokable& ) const;
    bool operator!=( const QskMetaInvokable& ) const;

    explicit operator bool() const;

    Type type() const;
    bool isNull() const;

    QVector< int > parameterTypes() const;
    int returnType() const;

    void invoke( QObject*, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    void reset();

    QMetaMethod method() const;    
    QMetaProperty property() const;    
    QskMetaFunction function() const;    

private:
    struct FunctionData
    {
        void* functionCall;
    };

    struct MetaData
    {
        const QMetaObject* metaObject;
        int index; // method or property index
    };

    union
    {
        FunctionData m_functionData;
        MetaData m_metaData;
    };

    int m_type : 3;
};

inline QskMetaInvokable::QskMetaInvokable():
    m_type( Invalid )
{
}

inline bool QskMetaInvokable::operator!=( const QskMetaInvokable& other ) const
{
    return !( *this == other );
}

inline QskMetaInvokable::operator bool() const
{
    return !isNull();
}

inline QskMetaInvokable::Type QskMetaInvokable::type() const
{
    return static_cast< Type >( m_type );
}

Q_DECLARE_METATYPE( QskMetaInvokable )

#endif
