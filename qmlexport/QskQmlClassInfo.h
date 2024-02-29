/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_QML_CLASSINFO_H
#define QSK_QML_CLASSINFO_H

#include <qqml.h>

class QByteArray;

#if QT_VERSION < QT_VERSION_CHECK( 6, 3, 0 )
    #define QSK_STRUCT_VERSION 0
#elif QT_VERSION < QT_VERSION_CHECK( 6, 5, 0 )
    #define QSK_STRUCT_VERSION 1
#else
    #define QSK_STRUCT_VERSION 2
#endif

namespace QskQml
{
    class ClassInfo
    {
      public:
        ClassInfo( const char* qmlName, const QMetaObject* );
        template< typename T > void setTypeInfo();

        int registerType();

      private:
        static QByteArray normalizedListName(
            const char* containerName, const QMetaObject& );

        QQmlPrivate::RegisterType m_info;
    };

    template< typename T >
    inline void ClassInfo::setTypeInfo()
    {
        using namespace QQmlPrivate;

        constexpr bool isObject = std::is_base_of_v< QObject, T >;

        if ( isObject )
            m_info.typeId = QMetaType::fromType< T* >();
        else
            m_info.typeId = QMetaType::fromType< T >();

        m_info.objectSize = sizeof( T );
        m_info.create = Constructors< T >::createInto;

        m_info.createValueType = ValueType< T, void >::create;

        const auto name = normalizedListName(
            isObject ? "QQmlListProperty" : "QList", T::staticMetaObject );

        /*
            QMetaType::fromType< QList< T >() creates a lot of symbols
            that end up in QskQml.o for all gadgets. So we export only
            registered lists. Registration might be done in the qskinny library
            itself - or in QskQml.cpp.

            As we do not design plain data being a QObject I'm not sure
            if we need to have QQmlListProperty< T > at all ...
         */
        m_info.listId = QMetaType::fromName( name.constData() );

        m_info.parserStatusCast = StaticCastSelector< T, QQmlParserStatus >::cast();
        m_info.valueSourceCast = StaticCastSelector< T, QQmlPropertyValueSource >::cast();
        m_info.valueInterceptorCast = StaticCastSelector< T, QQmlPropertyValueInterceptor >::cast();

#if QSK_STRUCT_VERSION >= 1
        m_info.finalizerCast = StaticCastSelector< T, QQmlFinalizerHook >::cast();
#endif
    }

    inline int ClassInfo::registerType()
    {
        return qmlregister( QQmlPrivate::TypeRegistration, &m_info );
    }
}

#endif
