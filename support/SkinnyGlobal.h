/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

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
