/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ListBox.h"
#include <QskFunctions.h>

ListBox::ListBox( QQuickItem* parent )
    : Inherited( parent )
{
    setAlternatingRowColors( true );

    // increasing the padding of each row: usually the job of the skin !
    setPaddingHint( Cell, QMargins( 10, 20, 10, 20 ) );

    populate();

    setSelectedRow( 5 );
}

void ListBox::populate()
{
    const int count = 10000;

    const QString format( "Row %1: The quick brown fox jumps over the lazy dog" );

    QStringList entries;
    entries.reserve( count );

    for ( int i = 0; i < count; i++ )
    {
        entries += format.arg( i + 1 );
    }

    // we know, that the last entry is the longest one and
    // can prevent the list box from having to find it out
    // the expensive way.

    const qreal maxWidth = qskHorizontalAdvance( effectiveFont( Cell ), entries.last() );
    setColumnWidthHint( 0, maxWidth );

    append( entries );
}
