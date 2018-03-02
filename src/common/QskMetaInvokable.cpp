/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaInvokable.h"

namespace
{
    // to have access to the private section of QSlotObjectBase
    struct SlotObject
    {
        QAtomicInt ref;
        QskMetaInvokable::InvokeFunction invoke;
        const int* parameterTypes;
    };

    static_assert( sizeof( SlotObject ) == sizeof( QskMetaInvokable ),
        "Bad cast: QskMetaInvokable does not match" );
}

int QskMetaInvokable::typeInfo() const
{
    auto that = const_cast< QskMetaInvokable* >( this );

    int value;

    reinterpret_cast< SlotObject* >( that )->invoke( TypeInfo, that,
        nullptr, reinterpret_cast< void** >( &value ), nullptr );

    return value;
}

int QskMetaInvokable::refCount() const
{
    auto that = const_cast< QskMetaInvokable* >( this );
    return reinterpret_cast< SlotObject* >( that )->ref.load();
}
