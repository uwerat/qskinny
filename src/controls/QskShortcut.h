/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHORTCUT_H
#define QSK_SHORTCUT_H

#include "QskGlobal.h"
#include <QObject>

class QQuickWindow;
class QQuickItem;
class QKeySequence;

class QSK_EXPORT QskShortcut
{
public:
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    static int addShortcut( QQuickWindow*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    static int addShortcut( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject* receiver, const char* method );

    // shortcut calling a QObject method
    template< typename Func1 >
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot );

    // shortcut calling a functor or function pointer
    template< typename Func1 >
    static int addShortcut( const QKeySequence&, bool autoRepeat,
        const QObject* context, Func1 slot );

    // shortcut calling a functor or function pointer
    template< typename Func1 >
    static int addShortcut( const QKeySequence&, bool autoRepeat, Func1 slot );

    static void setAutoRepeat( int, bool on );
    static void setEnabled( int, bool on );

private:
    QskShortcut() = delete;
    ~QskShortcut() = delete;

    static int addShortcutImpl( const QKeySequence&, bool autoRepeat,
        const QObject* receiver, QtPrivate::QSlotObjectBase* );
};

// shortcut calling a QObject method
template< typename Func1 >
inline int QskShortcut::addShortcut( const QKeySequence& key, bool autoRepeat,
    const typename QtPrivate::FunctionPointer< Func1 >::Object* receiver, Func1 slot )
{
    typedef QtPrivate::FunctionPointer< Func1 > SlotType;

    Q_STATIC_ASSERT_X( int( SlotType::ArgumentCount ) == 0,
        "The slot must not have any arguments.");

    auto slotObj = new QtPrivate::QSlotObject< Func1,
            typename SlotType::Arguments, void >(slot);

    return addShortcutImpl( key, autoRepeat, receiver, slotObj );
}

// shortcut calling a functor or function pointer
template< typename Func1 >
int QskShortcut::addShortcut( const QKeySequence& key, bool autoRepeat,
    const QObject* context, Func1 slot )
{
    using namespace QtPrivate;

    typedef FunctionPointer< Func1 > SlotType;

    Q_STATIC_ASSERT_X( int( SlotType::ArgumentCount ) <= 0,
        "The slot must not have any arguments.");

    Q_STATIC_ASSERT_X( !SlotType::IsPointerToMemberFunction,
        "The slot must be no member function." );

    auto slotObj = new QFunctorSlotObject< Func1, 0,
            typename List_Left< void, 0 >::Value, void >( std::move( slot ) );

    return addShortcutImpl( key, autoRepeat, context, slotObj );
}

// shortcut calling a functor or function pointer
template< typename Func1 >
int QskShortcut::addShortcut( const QKeySequence& key, bool autoRepeat, Func1 slot )
{
    return addShortcut( key, autoRepeat, nullptr, slot );
}

#endif
