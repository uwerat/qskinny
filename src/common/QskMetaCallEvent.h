/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_META_CALL_EVENT_H
#define QSK_META_CALL_EVENT_H

#include "QskGlobal.h"
#include "QskInternalMacros.h"
#include <qmetaobject.h>

/*
    The API for sending QMetaCallEvents has significantly changed
    with Qt 6.11. The methods below ( only internal ! ) hide
    those differences.
 */

class QMetaCallEvent;

#if QT_CONFIG(thread)

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )

    QSK_QT_PRIVATE_BEGIN
    #include <private/qlatch_p.h>
    QSK_QT_PRIVATE_END

    class QskMetaCallLatch : public QLatch { public: QskMetaCallLatch() : QLatch( 1 ) {} };

#else
    #include <qsemaphore.h>
    class QskMetaCallLatch : public QSemaphore { public: void wait() { acquire(); } };
#endif

#endif

extern QMetaCallEvent* qskCreateMetaObjectCallEvent(
    QMetaObject::Call, const QMetaObject*,
    ushort offset, ushort index, void* args[], QskMetaCallLatch* );

extern QMetaCallEvent* qskCreateQueuedMethodCallEvent(
    const QMetaObject*, ushort offset, ushort index, void* args[] );

extern QMetaCallEvent* qskCreateQueuedWritePropertyCallEvent(
    const QMetaObject*, ushort offset, ushort index, void* args[] );

extern QMetaCallEvent* qskCreateQueuedFunctorCallEvent(
    void* functionCall, int argc, void* args[] );

#endif
