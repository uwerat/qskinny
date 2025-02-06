/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_INTERNAL_MACROS_H
#define QSK_INTERNAL_MACROS_H

#include <qcompilerdetection.h>

#define QSK_QT_PRIVATE_BEGIN \
    QT_WARNING_PUSH \
    QT_WARNING_DISABLE_GCC("-Wpragmas") \
    QT_WARNING_DISABLE_GCC("-Wpedantic") \
    QT_WARNING_DISABLE_GCC("-Wsuggest-override") \
    QT_WARNING_DISABLE_GCC("-Wsuggest-final-types") \
    QT_WARNING_DISABLE_GCC("-Wsuggest-final-methods")

#define QSK_QT_PRIVATE_END \
    QT_WARNING_POP

#define QSK_HIDDEN_EXTERNAL_BEGIN \
    QT_WARNING_PUSH \
    QT_WARNING_DISABLE_GCC("-Wmissing-declarations")

#define QSK_HIDDEN_EXTERNAL_END \
    QT_WARNING_POP

#endif
