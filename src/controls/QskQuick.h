/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_QUICK_H
#define QSK_QUICK_H

#include "QskGlobal.h"
#include "QskPlacementPolicy.h"

#include <qnamespace.h>
#include <qquickitem.h>

class QskSizePolicy;

class QSGNode;
class QSGTransformNode;
class QSGRootNode;
class QRectF;
class QRhi;

template< typename T > class QList;

/*
    Exporting useful methods from QQuickItemPrivate/QQuickWindowPrivate
 */

QSK_EXPORT QRhi* qskRenderingHardwareInterface( const QQuickWindow* );

QSK_EXPORT bool qskIsItemInDestructor( const QQuickItem* );
QSK_EXPORT bool qskIsItemComplete( const QQuickItem* );
QSK_EXPORT bool qskIsAncestorOf( const QQuickItem* item, const QQuickItem* child );
QSK_EXPORT bool qskIsTabFence( const QQuickItem* );
QSK_EXPORT bool qskIsShortcutScope( const QQuickItem* );
QSK_EXPORT bool qskIsVisibleTo( const QQuickItem* item, const QQuickItem* ancestor );
QSK_EXPORT bool qskIsVisibleToParent( const QQuickItem* );
QSK_EXPORT bool qskIsPolishScheduled( const QQuickItem* );

QSK_EXPORT bool qskIsVisibleToLayout( const QQuickItem* );
QSK_EXPORT bool qskIsAdjustableByLayout( const QQuickItem* );

QSK_EXPORT QSizeF qskEffectiveSizeHint( const QQuickItem*,
    Qt::SizeHint, const QSizeF& constraint = QSizeF() );

QSK_EXPORT QSizeF qskSizeConstraint( const QQuickItem*,
    Qt::SizeHint, const QSizeF& constraint = QSizeF() );

QSK_EXPORT QSizeF qskConstrainedItemSize( const QQuickItem*, const QSizeF& );
QSK_EXPORT QRectF qskConstrainedItemRect(
    const QQuickItem*, const QRectF&, Qt::Alignment );

QSK_EXPORT QskSizePolicy qskSizePolicy( const QQuickItem* );
QSK_EXPORT Qt::Alignment qskLayoutAlignmentHint( const QQuickItem* );

QSK_EXPORT QskPlacementPolicy qskPlacementPolicy( const QQuickItem* );
QSK_EXPORT void qskSetPlacementPolicy( QQuickItem*, QskPlacementPolicy );

QSK_EXPORT QskPlacementPolicy::Policy qskEffectivePlacementPolicy( const QQuickItem* );

QSK_EXPORT QRectF qskItemRect( const QQuickItem* );

QSK_EXPORT QRectF qskItemGeometry( const QQuickItem* );
QSK_EXPORT void qskSetItemGeometry( QQuickItem*, const QRectF& );

QSK_EXPORT QQuickItem* qskNearestFocusScope( const QQuickItem* );
QSK_EXPORT void qskForceActiveFocus( QQuickItem*, Qt::FocusReason );

QSK_EXPORT QList< QQuickItem* > qskPaintOrderChildItems( const QQuickItem* );

QSK_EXPORT void qskUpdateInputMethod( const QQuickItem*, Qt::InputMethodQueries );
QSK_EXPORT void qskInputMethodSetVisible( const QQuickItem*, bool );

QSK_EXPORT const QSGTransformNode* qskItemNode( const QQuickItem* );
QSK_EXPORT const QSGNode* qskPaintNode( const QQuickItem* );

QSK_EXPORT const QSGRootNode* qskScenegraphAnchorNode( const QQuickItem* );
QSK_EXPORT const QSGRootNode* qskScenegraphAnchorNode( const QQuickWindow* );
QSK_EXPORT void qskSetScenegraphAnchor( QQuickItem*, bool on, bool hide = false );

QSK_EXPORT void qskItemUpdateRecursive( QQuickItem* );

QSK_EXPORT bool qskGrabMouse( QQuickItem* );
QSK_EXPORT void qskUngrabMouse( QQuickItem* );
QSK_EXPORT bool qskIsMouseGrabber( const QQuickItem* );

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
    return qskFindAncestorOf< T >( const_cast< QQuickItem* >( item ) );
}

inline qreal qskHeightForWidth(
    const QQuickItem* item, Qt::SizeHint which, qreal width )
{
    return qskEffectiveSizeHint(
        item, which, QSizeF( width, -1.0 ) ).height();
}

inline qreal qskWidthForHeight(
    const QQuickItem* item, Qt::SizeHint which, qreal height )
{
    return qskEffectiveSizeHint(
        item, which, QSizeF( -1.0, height ) ).width();
}

inline QRectF qskConstrainedItemRect( const QQuickItem* item, const QRectF& rect )
{
    return qskConstrainedItemRect( item, rect, qskLayoutAlignmentHint( item ) );
}

#endif
