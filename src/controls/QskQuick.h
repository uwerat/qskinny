/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_QUICK_H
#define QSK_QUICK_H 1

#include "QskGlobal.h"
#include <QQuickItem>

/*
    Exporting methods from QQuickItemPrivate, that should be part
    of QQuickItem.
 */

QSK_EXPORT bool qskIsItemComplete( const QQuickItem* item );
QSK_EXPORT bool qskIsAncestorOf( const QQuickItem* item, const QQuickItem* child );
QSK_EXPORT bool qskIsTransparentForPositioner( const QQuickItem* );
QSK_EXPORT bool qskIsTabFence( const QQuickItem* );
QSK_EXPORT bool qskIsShortcutScope( const QQuickItem* );

QSK_EXPORT QRectF qskItemRect( const QQuickItem* );

QSK_EXPORT QRectF qskItemGeometry( const QQuickItem* );
QSK_EXPORT void qskSetItemGeometry( QQuickItem*, const QRectF& );

QSK_EXPORT QQuickItem* qskNearestFocusScope( const QQuickItem* );
QSK_EXPORT void qskForceActiveFocus( QQuickItem*, Qt::FocusReason );

QSK_EXPORT QList< QQuickItem* > qskPaintOrderChildItems( const QQuickItem* );

QSK_EXPORT void qskUpdateInputMethod( const QQuickItem*, Qt::InputMethodQueries );

QSK_EXPORT const QSGNode* qskItemNode( const QQuickItem* );
QSK_EXPORT const QSGNode* qskPaintNode( const QQuickItem* );

#endif
