/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFontRole.h"
#include <qhash.h>

static void qskRegisterFontRole()
{
    qRegisterMetaType< QskFontRole >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskFontRole >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterFontRole )

QskHashValue QskFontRole::hash( QskHashValue seed ) const noexcept
{
    const auto v = static_cast< ushort >( category() )
        | ( static_cast< ushort >( emphasis() ) << 8 );

    return qHash( v, seed );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskFontRole& fontRole )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "FontRole( " << fontRole.category()
        << ", " << fontRole.emphasis() << " )";

    return debug;
}

#endif

#include "moc_QskFontRole.cpp"
