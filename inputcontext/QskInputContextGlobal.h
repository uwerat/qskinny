/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_INPUTCONTEXT_GLOBAL_H
#define QSK_INPUTCONTEXT_GLOBAL_H

#include "QskGlobal.h"

#ifdef QSK_DLL

#if defined( QSK_INPUTCONTEXT_MAKEDLL )     // create a DLL library
#define QSK_INPUTCONTEXT_EXPORT Q_DECL_EXPORT
#else                        // use a DLL library
#define QSK_INPUTCONTEXT_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef QSK_INPUTCONTEXT_EXPORT
#define QSK_INPUTCONTEXT_EXPORT
#endif

#endif
