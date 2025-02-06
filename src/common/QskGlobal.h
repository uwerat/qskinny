/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GLOBAL_H
#define QSK_GLOBAL_H

#include <qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK( 5, 15, 0 )
    static_assert( false, "QSkinny requires Qt >= 5.15" );
#endif

// QSK_VERSION is (major << 16) + (minor << 8) + patch.

#define QSK_VERSION       0x000001
#define QSK_VERSION_STR   "0.0.1"

#ifdef QSK_DLL

#if defined( QSK_MAKEDLL )     // create a DLL library
    #define QSK_EXPORT Q_DECL_EXPORT
#else                        // use a DLL library
    #define QSK_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_EXPORT
    #define QSK_EXPORT
#endif

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    using QskHashValue = uint;
#else
    using QskHashValue = size_t;
#endif

#endif
