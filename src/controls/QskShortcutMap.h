/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHORTCUT_MAP_H
#define QSK_SHORTCUT_MAP_H

#include "QskMetaFunction.h"
#include <qquickwindow.h>

class QQuickItem;
class QKeySequence;

class QSK_EXPORT QskShortcutMap
{
  public:
    static void setAutoRepeat( int, bool on );
    static void setEnabled( int, bool on );
    static void setEnabled( const QKeySequence&, bool on );

    static void removeShortcut( int );

    // string based slots
    static int addShortcut( const QKeySequence&,
        bool autoRepeat, const QObject* receiver, const char* method );

    static int addShortcut( QQuickWindow*, const QKeySequence&,
        bool autoRepeat, const QObject* receiver, const char* method );

    static int addShortcut( QQuickItem*, const QKeySequence&,
        bool autoRepeat, const QObject* receiver, const char* method );

    // functor based slots
    template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* = nullptr >
    static int addShortcut( const QKeySequence&, bool autoRepeat, T function );

    template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* = nullptr >
    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat, T function );

    template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* = nullptr >
    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat, T function );

    template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* = nullptr >
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const QObject* context, T function );

    template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* = nullptr >
    static int addShortcut( QQuickItem*, const QKeySequence&,
        bool autoRepeat, const QObject* context, T function );

    template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* = nullptr >
    static int addShortcut( QQuickWindow*, const QKeySequence&,
        bool autoRepeat, const QObject* context, T function );

    template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* = nullptr >
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< T >::Object* receiver, T function );

    template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* = nullptr >
    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< T >::Object* receiver, T function );

    template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* = nullptr >
    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< T >::Object* receiver, T function );

    // calling the registered callbacks manually
    static bool invokeCallback( const QKeySequence& );
    static bool invokeCallback( QQuickWindow*, const QKeySequence& );
    static bool invokeCallback( QQuickItem*, const QKeySequence& );

    static bool contextMatcher( const QQuickItem*, Qt::ShortcutContext );

  private:
    QskShortcutMap() = delete;
    ~QskShortcutMap() = delete;

    template< typename T >
    static int addFunctionT(
        QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, T );

    static int addFunction(
        QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const QskMetaFunction& );

    static int addMethod(
        QQuickItem*, const QKeySequence&, bool autoRepeat,
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

template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* >
inline int QskShortcutMap::addShortcut(
    const QKeySequence& sequence, bool autoRepeat, T function )
{
    return addFunctionT( nullptr, sequence, autoRepeat, nullptr, function );
}

template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* >
inline int QskShortcutMap::addShortcut(
    QQuickItem* item, const QKeySequence& sequence,
    bool autoRepeat, T function )
{
    return addFunctionT( item, sequence, autoRepeat, nullptr, function );
}

template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* >
inline int QskShortcutMap::addShortcut(
    QQuickWindow* window, const QKeySequence& sequence,
    bool autoRepeat, T function )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunctionT( item, sequence, autoRepeat, nullptr, function );
}

template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* >
inline int QskShortcutMap::addShortcut(
    const QKeySequence& sequence, bool autoRepeat,
    const QObject* context, T function )
{
    return addFunctionT( nullptr, sequence, autoRepeat, context, function );
}

template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* >
inline int QskShortcutMap::addShortcut(
    QQuickItem* item, const QKeySequence& sequence, bool autoRepeat,
    const QObject* context, T function )
{
    return addFunctionT( item, sequence, autoRepeat, context, function );
}

template< typename T, QskMetaFunctionTraits::IsFunctorOrStaticFunction< T >* >
inline int QskShortcutMap::addShortcut(
    QQuickWindow* window, const QKeySequence& sequence,
    bool autoRepeat, const QObject* context, T function )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunctionT( item, sequence, autoRepeat, context, function );
}

template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* >
inline int QskShortcutMap::addShortcut(
    const QKeySequence& sequence, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< T >::Object* receiver, T function )
{
    return addFunctionT( nullptr, sequence, autoRepeat, receiver, function );
}

template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* >
inline int QskShortcutMap::addShortcut(
    QQuickItem* item, const QKeySequence& sequence, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< T >::Object* receiver, T function )
{
    return addFunctionT( item, sequence, autoRepeat, receiver, function );
}

template< typename T, QskMetaFunctionTraits::IsMemberFunction< T >* >
inline int QskShortcutMap::addShortcut(
    QQuickWindow* window, const QKeySequence& sequence, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< T >::Object* receiver, T function )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunctionT( item, sequence, autoRepeat, receiver, function );
}

template< typename T >
inline int QskShortcutMap::addFunctionT(
    QQuickItem* item, const QKeySequence& sequence, bool autoRepeat,
    const QObject* context, T function )
{
    static_assert( QskMetaFunctionTraits::argumentCount< T >() == 0,
        "QskShortcutMap::addShortcut: #number of arguments need to be 0." );

    return addFunction( item, sequence, autoRepeat, context, function );
}

#endif
