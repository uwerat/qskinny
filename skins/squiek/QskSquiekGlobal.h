/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SQUIEK_GLOBAL_H
#define QSK_SQUIEK_GLOBAL_H

#include "QskGlobal.h"

#ifdef QSK_DLL

#if defined( QSK_SQUIEK_MAKEDLL )     // create a DLL library
#define QSK_SQUIEK_EXPORT Q_DECL_EXPORT
#else                        // use a DLL library
#define QSK_SQUIEK_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_SQUIEK_EXPORT
#define QSK_SQUIEK_EXPORT
#endif

#endif
