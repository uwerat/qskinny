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

/*
    QML need methods being Q_INVOKABLE and QQuickItem parameters
    need to be non const.
 */

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

    Q_INVOKABLE void setAlignment( QQuickItem* item, Qt::Alignment alignment )
    {
        LayoutBox::setAlignment( item, alignment );
    }

    Q_INVOKABLE Qt::Alignment alignment( QQuickItem* item ) const
    {
        return LayoutBox::alignment( item );
    }
};

class QskStackBoxQml : public QskLayoutBoxQml< QskStackBox >
{
    Q_OBJECT

    Q_INVOKABLE void setAlignment( int index, Qt::Alignment alignment )
    {
        QskStackBox::setAlignment( index, alignment );
    }

    Q_INVOKABLE Qt::Alignment alignment( int index ) const
    {
        return QskStackBox::alignment( index );
    }
};

class QskLinearBoxQml : public QskLayoutBoxQml< QskLinearBox >
{
    Q_OBJECT

  public:

    Q_INVOKABLE void setAlignment( int index, Qt::Alignment alignment )
    {
        QskLinearBox::setAlignment( index, alignment );
    }

    Q_INVOKABLE Qt::Alignment alignment( int index ) const
    {
        return QskLinearBox::alignment( index );
    }

    Q_INVOKABLE void setStretchFactor( QQuickItem* item, int stretchFactor )
    {
        QskLinearBox::setStretchFactor( item, stretchFactor );
    }

    Q_INVOKABLE int stretchFactor( QQuickItem* item ) const
    {
        return QskLinearBox::stretchFactor( item );
    }

    Q_INVOKABLE void setRetainSizeWhenHidden( QQuickItem* item, bool on )
    {
        QskLinearBox::setRetainSizeWhenHidden( item, on );
    }

    Q_INVOKABLE bool retainSizeWhenHidden( QQuickItem* item ) const
    {
        return QskLinearBox::retainSizeWhenHidden( item );
    }
};

class QskGridBoxQml : public QskLayoutBoxQml< QskGridBox >
{
    Q_OBJECT

  public:

    Q_INVOKABLE bool retainSizeWhenHidden( QQuickItem* item ) const
    {
        return QskGridBox::retainSizeWhenHidden( item );
    }

    Q_INVOKABLE void setRetainSizeWhenHidden( QQuickItem* item, bool on )
    {
        QskGridBox::setRetainSizeWhenHidden( item, on );
    }
};

#endif
