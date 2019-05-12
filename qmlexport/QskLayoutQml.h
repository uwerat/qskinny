/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_QML_H
#define QSK_LAYOUT_QML_H

#include "QskQmlGlobal.h"

#include <QskStackBox.h>
#include <QskLinearBox.h>
#include <QskGridBox.h>

template< typename LayoutBox >
class QskLayoutBoxQml : public LayoutBox
{
  public:

    Q_INVOKABLE bool isEmpty() const
    {
        return LayoutBox::isEmpty();
    }

    Q_INVOKABLE QQuickItem* itemAtIndex( int index ) const
    {
        return LayoutBox::itemAtIndex( index );
    }

    Q_INVOKABLE int indexOf( const QQuickItem* item ) const
    {
        return LayoutBox::indexOf( item );
    }
    
    Q_INVOKABLE void removeAt( int index )
    {
        return LayoutBox::removeAt( index );
    }

    Q_INVOKABLE void removeItem( QQuickItem* item )
    {
        // QML does not like a const version
        LayoutBox::removeItem( item );
    }   
};

class QskStackBoxQml : public QskLayoutBoxQml< QskLinearBox > { Q_OBJECT };
class QskLinearBoxQml : public QskLayoutBoxQml< QskLinearBox > { Q_OBJECT };
class QskGridBoxQml : public QskLayoutBoxQml< QskGridBox > { Q_OBJECT };

#endif
