/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSizePolicy.h"

QskSizePolicy::Policy QskSizePolicy::policy( Qt::Orientation orientation ) const
{
    return ( orientation == Qt::Horizontal )
           ? horizontalPolicy() : verticalPolicy();
}

void QskSizePolicy::setPolicy( Qt::Orientation orientation, Policy policy )
{
    if ( orientation == Qt::Horizontal )
        setHorizontalPolicy( policy );
    else
        setVerticalPolicy( policy );
}

QskSizePolicy::ConstraintType QskSizePolicy::constraintType() const
{
    if ( horizontalPolicy() & ConstrainedFlag )
        return QskSizePolicy::WidthForHeight;

    if ( verticalPolicy() & ConstrainedFlag )
        return QskSizePolicy::HeightForWidth;
        
    return QskSizePolicy::Unconstrained;
}   

Qt::SizeHint QskSizePolicy::effectiveSizeHintType(
    Qt::SizeHint which, Qt::Orientation orientation ) const
{
    const auto policy = ( orientation == Qt::Horizontal )
        ? horizontalPolicy() : verticalPolicy();

    if ( which == Qt::MinimumSize )
    {
        if ( !( policy & ShrinkFlag ) )
            return Qt::PreferredSize;
    }
    else if ( which == Qt::MaximumSize )
    {
        if ( !( policy & ( GrowFlag | ExpandFlag ) ) )
            return Qt::PreferredSize;
    }

    return which;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskSizePolicy& policy )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "SizePolicy" << '(';
    debug << policy.horizontalPolicy() << ", " << policy.verticalPolicy();
    debug << ')';

    return debug;
}

#endif

#include "moc_QskSizePolicy.cpp"
