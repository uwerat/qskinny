/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_QML_H
#define QSK_LAYOUT_QML_H

#include "QskQmlGlobal.h"

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
};

class QskLinearBoxQml : public QskLayoutBoxQml< QskLinearBox >
{
    Q_OBJECT

  public:

    Q_INVOKABLE void setStretchFactor( QQuickItem* item, int stretchFactor )
    {
        QskLinearBox::setStretchFactor( item, stretchFactor );
    }

    Q_INVOKABLE int stretchFactor( QQuickItem* item ) const
    {
        return QskLinearBox::stretchFactor( item );
    }
};

class QskGridBoxQml : public QskLayoutBoxQml< QskGridBox >
{
    Q_OBJECT

    Q_PROPERTY( qreal horizontalSpacing READ horizontalSpacing
        WRITE setHorizontalSpacing RESET resetHorizontalSpacing
        NOTIFY horizontalSpacingChanged )

    Q_PROPERTY( qreal verticalSpacing READ verticalSpacing
        WRITE setVerticalSpacing RESET resetVerticalSpacing
        NOTIFY verticalSpacingChanged )

  public:
    void setHorizontalSpacing( qreal );
    void resetHorizontalSpacing();
    qreal horizontalSpacing() const;

    void setVerticalSpacing( qreal );
    void resetVerticalSpacing();
    qreal verticalSpacing() const;

  Q_SIGNALS:
    void verticalSpacingChanged();
    void horizontalSpacingChanged();
};

#endif
