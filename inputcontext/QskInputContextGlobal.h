/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
