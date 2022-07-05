/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
