/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHORTCUT_MAP_H
#define QSK_SHORTCUT_MAP_H

#include "QskGlobal.h"
#include "QskMetaFunction.h"

#include <QQuickWindow>

class QQuickItem;
class QKeySequence;

class QSK_EXPORT QskShortcutMap
{
public:
    static void setAutoRepeat( int, bool on );
    static void setEnabled( int, bool on );

    static void removeShortcut( int );

    // string based slots
    static int addShortcut( const QKeySequence&,
        bool autoRepeat, const QObject* receiver, const char* method );

    static int addShortcut( QQuickWindow*, const QKeySequence&,
        bool autoRepeat, const QObject* receiver, const char* method );

    static int addShortcut( QQuickItem*, const QKeySequence&,
        bool autoRepeat, const QObject* receiver, const char* method );

    // functor based slots
    static int addShortcut( const QKeySequence&,
        bool autoRepeat, const QskMetaFunction& );

    static int addShortcut( const QKeySequence&,
        bool autoRepeat, const QObject* context, const QskMetaFunction& );

    static int addShortcut( QQuickItem*, const QKeySequence&,
        bool autoRepeat, const QskMetaFunction& );

    static int addShortcut( QQuickItem*, const QKeySequence&,
        bool autoRepeat, const QObject* context, const QskMetaFunction& );

    static int addShortcut( QQuickWindow*, const QKeySequence&,
        bool autoRepeat, const QskMetaFunction& );

    static int addShortcut( QQuickWindow*, const QKeySequence&,
        bool autoRepeat, const QObject* context, const QskMetaFunction& );

    static bool contextMatcher( const QQuickItem*, Qt::ShortcutContext );

private:
    QskShortcutMap() = delete;
    ~QskShortcutMap() = delete;

    static int addFunction(
        QQuickItem* item, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const QskMetaFunction& );

    static int addMethod(
        QQuickItem* item, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* );
};

inline int QskShortcutMap::addShortcut(
    const QKeySequence& sequence, bool autoRepeat,
    const QObject* receiver, const char* method )
{
    return addMethod( nullptr, sequence, autoRepeat, receiver, method );
}

inline int QskShortcutMap::addShortcut(
    QQuickWindow* window, const QKeySequence& sequence,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    auto item = window ? window->contentItem() : nullptr;
    return addMethod( item, sequence, autoRepeat, receiver, method );
}

inline int QskShortcutMap::addShortcut(
    const QKeySequence& sequence, bool autoRepeat,
    const QskMetaFunction& function )
{
    return addFunction( nullptr, sequence, autoRepeat, nullptr, function );
}

inline int QskShortcutMap::addShortcut(
    const QKeySequence& sequence, bool autoRepeat,
    const QObject* context, const QskMetaFunction& function )
{
    return addFunction( nullptr, sequence, autoRepeat, context, function );
}

inline int QskShortcutMap::addShortcut(
    QQuickItem* item, const QKeySequence& sequence, bool autoRepeat,
    const QskMetaFunction& function )
{
    return addFunction( item, sequence, autoRepeat, nullptr, function );
}

inline int QskShortcutMap::addShortcut(
    QQuickItem* item, const QKeySequence& sequence, bool autoRepeat,
    const QObject* context, const QskMetaFunction& function )
{
    return addFunction( item, sequence, autoRepeat, context, function );
}

inline int QskShortcutMap::addShortcut(
    QQuickWindow* window, const QKeySequence& sequence, bool autoRepeat,
    const QskMetaFunction& function )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunction( item, sequence, autoRepeat, nullptr, function );
}

inline int QskShortcutMap::addShortcut(
    QQuickWindow* window, const QKeySequence& sequence, bool autoRepeat,
    const QObject* context, const QskMetaFunction& function )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunction( item, sequence, autoRepeat, context, function );
}

#endif
