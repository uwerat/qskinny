/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_GLOBAL_H
#define QSK_FLUENT2_GLOBAL_H

#include <QskGlobal.h>

#ifdef QSK_DLL

#if defined( QSK_FLUENT2_MAKEDLL )     // create a DLL library
#define QSK_FLUENT2_EXPORT Q_DECL_EXPORT
#else                        // use a DLL library
#define QSK_FLUENT2_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_FLUENT2_EXPORT
#define QSK_FLUENT2_EXPORT
#endif

#endif
