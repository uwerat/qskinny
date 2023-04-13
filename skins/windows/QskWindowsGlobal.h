/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_WINDOWS_GLOBAL_H
#define QSK_WINDOWS_GLOBAL_H

#include "QskGlobal.h"

#ifdef QSK_DLL

#if defined( QSK_WINDOWS_MAKEDLL )     // create a DLL library
#define QSK_WINDOWS_EXPORT Q_DECL_EXPORT
#else                        // use a DLL library
#define QSK_WINDOWS_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_WINDOWS_EXPORT
#define QSK_WINDOWS_EXPORT
#endif

#endif
