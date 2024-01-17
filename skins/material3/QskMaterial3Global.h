/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_GLOBAL_H
#define QSK_MATERIAL3_GLOBAL_H

#include "QskGlobal.h"

#ifdef QSK_DLL

#if defined( QSK_MATERIAL3_MAKEDLL )     // create a DLL library
#define QSK_MATERIAL3_EXPORT Q_DECL_EXPORT
#else                        // use a DLL library
#define QSK_MATERIAL3_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_MATERIAL3_EXPORT
#define QSK_MATERIAL3_EXPORT
#endif

#endif
