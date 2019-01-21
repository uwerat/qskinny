/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskObjectTree.h"

#include <qguiapplication.h>
#include <qquickitem.h>
#include <qquickwindow.h>

bool QskObjectTree::isRoot( const QObject* object )
{
    return object == QGuiApplication::instance();
}

QObjectList QskObjectTree::childNodes( const QObject* object )
{
    QObjectList children;

    if ( object == nullptr )
    {
        const auto windows = QGuiApplication::topLevelWindows();
        for ( auto window : windows )
            children += window;
    }
    else if ( object->isWindowType() )
    {
        const auto childObjects = object->children();

        for ( auto child : childObjects )
        {
            if ( child->isWindowType() )
                children += child;
        }

        if ( auto w = qobject_cast< const QQuickWindow* >( object ) )
        {
            // For some reason the window is not the parent of its contentItem()
            children += w->contentItem();
        }
    }
    else if ( auto item = qobject_cast< const QQuickItem* >( object ) )
    {
        const auto childItems = item->childItems();

        for ( auto child : childItems )
            children += child;
    }

    return children;
}

QObject* QskObjectTree::parentNode( const QObject* object )
{
    if ( object == nullptr )
        return nullptr;

    if ( object->isWindowType() )
    {
        if ( object->parent() == nullptr )
            return QGuiApplication::instance();
    }

    if ( auto item = qobject_cast< const QQuickItem* >( object ) )
    {
        if ( item->parentItem() )
            return item->parentItem();

        return item->window();
    }

    return object->parent();
}

void QskObjectTree::traverseDown( QObject* object, Visitor& visitor )
{
    const auto children = childNodes( object );
    for ( QObject* child : children )
    {
        const bool done = visitor.visitDown( child );
        if ( !done )
            traverseDown( child, visitor );
    }
}

void QskObjectTree::traverseUp( QObject* object, Visitor& visitor )
{
    QObject* parent = parentNode( object );
    if ( parent )
    {
        const bool done = visitor.visitUp( parent );
        if ( !done )
            traverseUp( parent, visitor );
    }
}
