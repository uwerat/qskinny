/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSizePolicy.h"
#include <QDebug>

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

#ifndef QT_NO_DEBUG_STREAM

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
