/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQuick.h"
#include "QskControl.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatforminputcontext.h>

QRectF qskItemRect( const QQuickItem* item )
{
    auto d = QQuickItemPrivate::get( item );
    return QRectF( 0, 0, d->width, d->height );
}

QRectF qskItemGeometry( const QQuickItem* item )
{
    auto d = QQuickItemPrivate::get( item );
    return QRectF( d->x, d->y, d->width, d->height );
}

void qskSetItemGeometry( QQuickItem* item, const QRectF& rect )
{
    if ( auto control = qobject_cast< QskControl* >( item ) )
    {
        control->setGeometry( rect );
    }
    else
    {
        item->setPosition( rect.topLeft() );
        item->setSize( rect.size() );
    }
}

bool qskIsItemComplete( const QQuickItem* item )
{
    return QQuickItemPrivate::get( item )->componentComplete;
}

bool qskIsAncestorOf( const QQuickItem* item, const QQuickItem* child )
{
    if ( item == nullptr || child == nullptr )
        return false;

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    return item->isAncestorOf( child );
#else
    while ( child )
    {
        if ( child == item )
            return true;

        child = child->parentItem();
    }

    return false;
#endif
}

bool qskIsTabFence( const QQuickItem* item )
{
    if ( item == nullptr )
        return false;

    return QQuickItemPrivate::get( item )->isTabFence;
}

bool qskIsShortcutScope( const QQuickItem* item )
{
    if ( item == nullptr )
        return false;

    /*
        We might have something like CTRL+W to close a "window".
        But in Qt/Quick a window is not necessarily a QQuickWindow
        like we have f.e QskSubWindow.

        Maybe it's worth to introduce a shortcutScope flag but for
        the moment we simply use the isFocusScope/isTabFence combination,
        that should usually be set for those "windows".
     */

    return item->isFocusScope() && QQuickItemPrivate::get( item )->isTabFence;
}

bool qskIsTransparentForPositioner( const QQuickItem* item )
{
    if ( item == nullptr )
        return true;

    return QQuickItemPrivate::get( item )->isTransparentForPositioner();
}

QQuickItem* qskNearestFocusScope( const QQuickItem* item )
{
    if ( item )
    {
        for ( QQuickItem* scope = item->parentItem();
            scope != nullptr; scope = scope->parentItem() )
        {
            if ( scope->isFocusScope() )
                return scope;
        }

        /*
            As the default setting of the root item is to be a focus scope
            we usually never get here - beside the flag has been explicitely
            disabled in application code.
         */
    }

    return nullptr;
}

void qskForceActiveFocus( QQuickItem* item, Qt::FocusReason reason )
{
    /*
        For unknown reasons Qt::PopupFocusReason is blocked inside of
        QQuickItem::setFocus and so we can't use QQuickItem::forceActiveFocus
     */

    if ( item == nullptr || item->window() == nullptr )
        return;

    auto wp = QQuickWindowPrivate::get( item->window() );

    while ( const auto scope = qskNearestFocusScope( item ) )
    {
        wp->setFocusInScope( scope, item, reason );
        item = scope;
    }
}

QQuickItem* qskInputContextItem()
{
    /*
        The item that is connected to the input context.
        - often the item having the active focus.
     */

    QQuickItem* inputItem = nullptr;

    /*
        We could also get the inputContext from QInputMethodPrivate
        but for some reason the gcc sanitizer reports errors
        when using it. So let's go with QGuiApplicationPrivate.
     */
    const auto inputContext =
        QGuiApplicationPrivate::platformIntegration()->inputContext();

    if ( inputContext && inputContext->isInputPanelVisible() )
    {
        /*
            QskInputContext allows to navigate inside the input panel
            without losing the connected input item
         */

        QMetaObject::invokeMethod( inputContext, "inputItem",
            Qt::DirectConnection, Q_RETURN_ARG( QQuickItem*, inputItem ) );
    }

    return inputItem;
}

void qskUpdateInputMethod( const QQuickItem* item, Qt::InputMethodQueries queries )
{
    if ( item == nullptr )
        return;

#if 1
    if ( !( item->flags() & QQuickItem::ItemAcceptsInputMethod ) )
        return;
#endif

    bool doUpdate;

    if ( const QQuickItem* inputItem = qskInputContextItem() )
        doUpdate = ( item == inputItem );
    else
        doUpdate = item->hasActiveFocus();

    if ( doUpdate )
        QGuiApplication::inputMethod()->update( queries );
}

QList< QQuickItem* > qskPaintOrderChildItems( const QQuickItem* item )
{
    if ( item )
        return QQuickItemPrivate::get( item )->paintOrderChildItems();

    return QList< QQuickItem* >();
}

const QSGNode* qskItemNode( const QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->itemNodeInstance;
}

const QSGNode* qskPaintNode( const QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->paintNode;
}
