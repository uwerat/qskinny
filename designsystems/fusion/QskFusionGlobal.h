/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FUSION_GLOBAL_H
#define QSK_FUSION_GLOBAL_H

#include "QskGlobal.h"

#ifdef QSK_DLL

#if defined( QSK_FUSION_MAKEDLL )  // create a DLL library
#define QSK_FUSION_EXPORT Q_DECL_EXPORT
#else                             // use a DLL library
#define QSK_FUSION_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_FUSION_EXPORT
#define QSK_FUSION_EXPORT
#endif

#endif
