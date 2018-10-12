/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SKINNY_GLOBAL_H
#define SKINNY_GLOBAL_H

#include <QskGlobal.h>

#ifdef QSK_DLL

#if defined( SKINNY_MAKEDLL )     // create a DLL library
#define SKINNY_EXPORT Q_DECL_EXPORT
#else                           // use a DLL library
#define SKINNY_EXPORT Q_DECL_IMPORT
#endif

#endif // QSK_DLL

#ifndef SKINNY_EXPORT
#define SKINNY_EXPORT
#endif

#endif
