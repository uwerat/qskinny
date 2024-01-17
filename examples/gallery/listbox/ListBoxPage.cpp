/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ListBoxPage.h"
#include <QskSimpleListBox.h>
#include <QskFunctions.h>

namespace
{
    class ListBox : public QskSimpleListBox
    {
      public:
        ListBox( QQuickItem* parent = nullptr )
            : QskSimpleListBox( parent )
        {
            setMargins( QMarginsF( 15, 10, 10, 10 ) );
            populate();

            setSelectedRow( 5 );
        }

      private:
        void populate()
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
    };
}

ListBoxPage::ListBoxPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setMargins( 5 );
    (void) new ListBox( this );
}
