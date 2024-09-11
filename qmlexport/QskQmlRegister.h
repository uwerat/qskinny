/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_QML_REGISTER_H
#define QSK_QML_REGISTER_H

#include "QskQmlModule.h"

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include "QskQmlClassInfo.h"
#endif

#include <qqml.h>

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    #define QSK_QML_REGISTER 0
#else
    #define QSK_QML_REGISTER 1
#endif

// Required for QFlags to be constructed from an enum value
#define QSK_REGISTER_FLAGS( Type ) \
    QMetaType::registerConverter< int, Type >( []( int value ) { return Type( value ); } )

namespace QskQml
{
    inline const char* classNameQml( const QMetaObject& metaObject )
    {
        // without the "Qsk" prefix
        return metaObject.className() + 3;
    }

    template< typename T >
    inline int registerUncreatableType( const char* qmlName )
    {
#if QSK_QML_REGISTER
        ClassInfo typeInfo( qmlName, &T::staticMetaObject );
        typeInfo.setTypeInfo< T >();

        return typeInfo.registerType();
#else
        return qmlRegisterUncreatableType< T >( QskQmlModule::name,
            QskQmlModule::version[0], QskQmlModule::version[1], qmlName, QString() );
#endif
    }

    inline int registerUncreatableMetaObject(
        const QMetaObject& staticMetaObject, const char* qmlName )
    {
#if QSK_QML_REGISTER
        ClassInfo typeInfo( qmlName, &staticMetaObject );
        return typeInfo.registerType();
#else
        return qmlRegisterUncreatableMetaObject( staticMetaObject,
            QskQmlModule::name, QskQmlModule::version[0], QskQmlModule::version[1],
            qmlName, QString() );
#endif
    }

    template< typename T >
    inline int registerObject( const char* qmlName = nullptr )
    {
        // the class name without the "Qsk" prefix
        if ( qmlName == nullptr )
            qmlName = classNameQml( T::staticMetaObject );

#if QSK_QML_REGISTER
        ClassInfo typeInfo( qmlName, &T::staticMetaObject );
        typeInfo.setTypeInfo< T >();

        return typeInfo.registerType();
#else
        return qmlRegisterType< T >( QskQmlModule::name,
            QskQmlModule::version[0], QskQmlModule::version[1], qmlName );
#endif
    }

    template< typename T >
    inline int registerGadget()
    {
        auto className = classNameQml( T::staticMetaObject );

#if QSK_QML_REGISTER
        /*
            According to the QML naming rules uncreatables have to
            start with a lowercase letter, while namespaces
            and creatable items usually start with a upper letter.
            This results in an odd naming scheme for the enums defined inside of gadgets.

            To work around this we register the gadget twice  - starting with
            upper or lower letter.

            Maybe it would make sense to only pass stripped metaObjects, where all
            enums are removed from the first and everything else than the enums from
            the second. TODO ...
         */

        if ( T::staticMetaObject.enumeratorCount() > 0 )
            registerUncreatableMetaObject( T::staticMetaObject, className );

        QByteArray name = className;
        name.data()[0] = std::tolower( name.data()[0] );

        return registerUncreatableType< T >( name.constData() );
#else
        return registerUncreatableType< T >( className );
#endif
    }

    inline int registerNamespace( const QMetaObject& metaObject )
    {
        return registerUncreatableMetaObject( metaObject, classNameQml( metaObject ) );
    }

    template< typename T >
    inline int registerSingleton( QObject* singleton )
    {
        const auto name = classNameQml( T::staticMetaObject );

        return qmlRegisterSingletonInstance( QskQmlModule::name,
            QskQmlModule::version[0], QskQmlModule::version[1], name, singleton );
    }
}

#ifdef QSK_QML_REGISTER
    #undef QSK_QML_REGISTER
#endif

#endif
