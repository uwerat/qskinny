/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskQmlClassInfo.h"
#include "QskQmlModule.h"
#include <cstring>

using namespace QskQml;

ClassInfo::ClassInfo( const char* qmlName, const QMetaObject* metaObject )
{
    m_info.structVersion = QSK_STRUCT_VERSION;

    m_info.objectSize = 0;
    m_info.create = nullptr;
    m_info.userdata = nullptr; // unused

    /*
        This one was introdued with Qt 6.x, but never worked
        as expected. With Qt 6.5 it has been replaced by adding
        the creationMethod that is triggering to look for
        invokable constructors.
        Let's check if it makes any sense to initialize it below
        at all. TODO ...
     */
    m_info.createValueType = nullptr;

    m_info.uri = QskQmlModule::name;
    m_info.version = QTypeRevision::fromVersion(
        QskQmlModule::version[0], QskQmlModule::version[1] );

    m_info.elementName = qmlName;
    m_info.metaObject = metaObject;

    /*
        We do not use attached properties as it always comes with
        creating extra QObjects.
     */
    m_info.attachedPropertiesFunction = nullptr;
    m_info.attachedPropertiesMetaObject = nullptr;

    m_info.parserStatusCast = m_info.valueSourceCast = m_info.valueInterceptorCast = -1;

    /*
        We do not use extensions as it always comes with
        creating extra QObjects.
     */
    m_info.extensionObjectCreate = nullptr;
    m_info.extensionMetaObject = nullptr;

    m_info.customParser = nullptr; // QQmlCustomParser, unused

    m_info.revision = QTypeRevision::zero();
    m_info.finalizerCast = -1;

    m_info.creationMethod = QQmlPrivate::ValueTypeCreationMethod::Structured;
}

QByteArray ClassInfo::normalizedListName(
    const char* containerName, const QMetaObject& metaObject )
{
    static QByteArray name;
    name.reserve( 256 );

    const int length1 = strlen( containerName );
    const int length2 = strlen( metaObject.className() );

    name.resize( length1 + length2 + 3 );

    auto p = name.data();

    memcpy( p, containerName, size_t( length1 ) );
    p += length1;

    *p++ = '<';

    memcpy( p, metaObject.className(), size_t( length2 ) );
    p += length2;

    *p++ = '>';
    *p++ = '\0';

    return name;
}
