/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GLOBAL_H
#define QSK_GLOBAL_H

#include <qcompilerdetection.h>
#include <qglobal.h>

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

#define QSK_QT_PRIVATE_BEGIN \
    QT_WARNING_PUSH \
    QT_WARNING_DISABLE_GCC("-Wpragmas") \
    QT_WARNING_DISABLE_GCC("-Wpedantic") \
    QT_WARNING_DISABLE_GCC("-Wsuggest-override") \
    QT_WARNING_DISABLE_GCC("-Wsuggest-final-types") \
    QT_WARNING_DISABLE_GCC("-Wsuggest-final-methods")

#define QSK_QT_PRIVATE_END \
    QT_WARNING_POP

#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )

#define qskAsConst qAsConst

#else

template< typename T >
struct QskAddConst { typedef const T Type; };

template< typename T >
constexpr typename QskAddConst< T >::Type& qskAsConst( T& t ) noexcept { return t; }

template< typename T >
void qskAsConst( const T&& ) = delete;

#endif

#ifdef Q_FALLTHROUGH
#define QSK_FALLTHROUGH Q_FALLTHROUGH
#else
#define QSK_FALLTHROUGH
#endif

#endif
