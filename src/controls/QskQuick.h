/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_QUICK_H
#define QSK_QUICK_H 1

#include "QskGlobal.h"
#include <qnamespace.h>
#include <qquickitem.h>

class QQuickItem;
class QSGNode;
class QRectF;
template< typename T > class QList;

/*
    Exporting methods from QQuickItemPrivate, that should be part
    of QQuickItem.
 */

QSK_EXPORT bool qskIsItemComplete( const QQuickItem* );
QSK_EXPORT bool qskIsAncestorOf( const QQuickItem* item, const QQuickItem* child );
QSK_EXPORT bool qskIsTabFence( const QQuickItem* );
QSK_EXPORT bool qskIsShortcutScope( const QQuickItem* );
QSK_EXPORT bool qskIsVisibleTo( const QQuickItem* item, const QQuickItem* ancestor );
QSK_EXPORT bool qskIsVisibleToParent( const QQuickItem* );
QSK_EXPORT bool qskIsPolishScheduled( const QQuickItem* );

QSK_EXPORT void qskSetTransparentForPositioner( QQuickItem*, bool );
QSK_EXPORT bool qskIsTransparentForPositioner( const QQuickItem* );

QSK_EXPORT QRectF qskItemRect( const QQuickItem* );

QSK_EXPORT QRectF qskItemGeometry( const QQuickItem* );
QSK_EXPORT void qskSetItemGeometry( QQuickItem*, const QRectF& );

QSK_EXPORT QQuickItem* qskNearestFocusScope( const QQuickItem* );
QSK_EXPORT void qskForceActiveFocus( QQuickItem*, Qt::FocusReason );

QSK_EXPORT QList< QQuickItem* > qskPaintOrderChildItems( const QQuickItem* );

QSK_EXPORT void qskUpdateInputMethod( const QQuickItem*, Qt::InputMethodQueries );
QSK_EXPORT void qskInputMethodSetVisible( const QQuickItem*, bool );

QSK_EXPORT const QSGNode* qskItemNode( const QQuickItem* );
QSK_EXPORT const QSGNode* qskPaintNode( const QQuickItem* );

inline void qskSetItemGeometry(
    QQuickItem* item, qreal x, qreal y, qreal width, qreal height )
{
    qskSetItemGeometry( item, QRectF( x, y, width, height ) );
}

template< typename T >
inline T qskFindAncestorOf( QQuickItem* item )
{
    for ( auto it = item; it != nullptr; it = it->parentItem() )
    {
        if ( auto ancestor = qobject_cast< T >( it ) )
            return ancestor;
    }

    return nullptr;
}

template< typename T >
inline T qskFindAncestorOf( const QQuickItem* item )
{
    return qskFindAncestorOf< std::remove_const< T > >(
        const_cast< QQuickItem * >( item ) );
}

#endif
