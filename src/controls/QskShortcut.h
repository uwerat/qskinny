/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHORTCUT_H
#define QSK_SHORTCUT_H

#include "QskGlobal.h"
#include <QQuickWindow>

class QQuickItem;
class QKeySequence;

class QSK_EXPORT QskShortcut
{
public:
    static void setAutoRepeat( int, bool on );
    static void setEnabled( int, bool on );

    static void removeShortcut( int );

    // -- traditional slots
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    // -- calling a QObject method
    template< typename Func1 >
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot );

    template< typename Func1 >
    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot );

    template< typename Func1 >
    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot );

    // -- calling a functor or function pointer inside a thread context
    template< typename Func1 >
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const QObject* context, Func1 slot );

    template< typename Func1 >
    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat,
        const QObject* context, Func1 slot );

    template< typename Func1 >
    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject* context, Func1 slot );

    // -- calling a functor or function pointer
    template< typename Func1 >
    static int addShortcut( const QKeySequence&, bool autoRepeat, Func1 slot );

    template< typename Func1 >
    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat, Func1 slot );

    template< typename Func1 >
    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat, Func1 slot );

private:
    QskShortcut() = delete;
    ~QskShortcut() = delete;

    static int addMethod( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    template< typename Func1 >
    static int addMemberSlot( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot );

    template< typename Func1 >
    static int addFunctorSlot( QQuickItem*, const QKeySequence&,
        bool autoRepeat, const QObject* context, Func1 slot );

    static int addSlotObject(
        QQuickItem* item, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, QtPrivate::QSlotObjectBase* );
};

template< typename Func1 >
inline int QskShortcut::addMemberSlot(
    QQuickItem* item, const QKeySequence& key, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot )
{
    using namespace QtPrivate;
    typedef FunctionPointer< Func1 > SlotType;

    Q_STATIC_ASSERT_X( int( SlotType::ArgumentCount ) == 0,
        "The slot must not have any arguments.");

    return addSlotObject( item, key, autoRepeat, receiver, 
        new QSlotObject< Func1, typename SlotType::Arguments, void >( slot ) );
}

template< typename Func1 >
inline int QskShortcut::addFunctorSlot(
    QQuickItem* item, const QKeySequence& key, bool autoRepeat,
    const QObject* context, Func1 slot )
{
    using namespace QtPrivate;
    typedef FunctionPointer< Func1 > SlotType;

    Q_STATIC_ASSERT_X( int( SlotType::ArgumentCount ) <= 0,
        "The slot must not have any arguments.");

    Q_STATIC_ASSERT_X( !SlotType::IsPointerToMemberFunction,
        "The slot must be no member function." );

    using Args = List_Left< void, 0 >::Value;

    return addSlotObject( item, key, autoRepeat, context, 
        new QFunctorSlotObject< Func1, 0, Args, void >( slot ) );
}

// -- traditional slots

inline int QskShortcut::addShortcut(
    QQuickItem* item, const QKeySequence& key, bool autoRepeat,
    const QObject* receiver, const char* method )
{
    return addMethod( item, key, autoRepeat, receiver, method );
}

inline int QskShortcut::addShortcut(
    const QKeySequence& key, bool autoRepeat,
    const QObject* receiver, const char* method )
{
    return addMethod( nullptr, key, autoRepeat, receiver, method );
}

inline int QskShortcut::addShortcut(
    QQuickWindow* window, const QKeySequence& key, bool autoRepeat,
    const QObject* receiver, const char* method )
{
    auto item = window ? window->contentItem() : nullptr;
    return addMethod( item, key, autoRepeat, receiver, method );
}

// -- calling a QObject method

template< typename Func1 >
inline int QskShortcut::addShortcut(
    QQuickItem* item, const QKeySequence& key, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot )
{
    return addMemberSlot( item, key, autoRepeat, receiver, slot );
}


template< typename Func1 >
inline int QskShortcut::addShortcut(
    QQuickWindow* window, const QKeySequence& key, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot )
{
    auto item = window ? window->contentItem() : nullptr;
    return addMemberSlot( item, key, autoRepeat, receiver, slot );
}

template< typename Func1 >
inline int QskShortcut::addShortcut(
    const QKeySequence& key, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot )
{
    return addMemberSlot( nullptr, key, autoRepeat, receiver, slot );
}

// -- calling a functor or function pointer with context

template< typename Func1 >
inline int QskShortcut::addShortcut(
    QQuickItem* item, const QKeySequence& key, bool autoRepeat,
    const QObject* context, Func1 slot )
{
    return addFunctorSlot( item, key, autoRepeat, context, slot );
}

template< typename Func1 >
inline int QskShortcut::addShortcut(
    QQuickWindow* window, const QKeySequence& key, bool autoRepeat,
    const QObject* context, Func1 slot )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunctorSlot( item, key, autoRepeat, context, slot );
}

template< typename Func1 >
inline int QskShortcut::addShortcut( const QKeySequence& key, bool autoRepeat,
    const QObject* context, Func1 slot )
{
    return addFunctorSlot( nullptr, key, autoRepeat, context, slot );
}

// -- calling a functor or function pointer

template< typename Func1 >
inline int QskShortcut::addShortcut(
    QQuickItem* item, const QKeySequence& key, bool autoRepeat, Func1 slot )
{
    return addFunctorSlot( item, key, autoRepeat, nullptr, slot );
}

template< typename Func1 >
inline int QskShortcut::addShortcut(
    QQuickWindow* window, const QKeySequence& key, bool autoRepeat, Func1 slot )
{
    auto item = window ? window->contentItem() : nullptr;
    return addFunctorSlot( item, key, autoRepeat, nullptr, autoRepeat, slot );
}

template< typename Func1 >
int QskShortcut::addShortcut( const QKeySequence& key, bool autoRepeat, Func1 slot )
{
    return addFunctorSlot( nullptr, key, autoRepeat, nullptr, slot );
}

#endif
