/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_H
#define QSK_LAYOUT_H

#include "QskControl.h"

class QskLayoutEngine;
class QskLayoutItem;

// might be derived from QskFrame
class QSK_EXPORT QskLayout : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    explicit QskLayout( QQuickItem* parent = 0 );
    ~QskLayout() override;

    Q_INVOKABLE bool isEmpty() const;

    Q_INVOKABLE int itemCount() const;
    Q_INVOKABLE QQuickItem* itemAtIndex( int index ) const;
    Q_INVOKABLE int indexOf( const QQuickItem* ) const;

    void removeItem( const QQuickItem* );
    Q_INVOKABLE void removeItem( QQuickItem* );
    Q_INVOKABLE void removeAt( int index );
    Q_INVOKABLE void clear();

    Q_INVOKABLE void setActive( bool );
    Q_INVOKABLE bool isActive() const;

    void adjustItem( const QQuickItem* );
    void adjustItemAt( int index );

  public Q_SLOTS:
    void activate();
    void invalidate();

  protected:
    bool event( QEvent* ) override;
    void geometryChangeEvent( QskGeometryChangeEvent* ) override;

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void updateLayout() override;

    QskLayoutEngine& engine();
    const QskLayoutEngine& engine() const;

    void setItemActive( const QQuickItem*, bool on );

    void insertItemInternal( QskLayoutItem*, int index );

    virtual void setupLayoutItem( QskLayoutItem*, int index );
    virtual void layoutItemInserted( QskLayoutItem*, int index );
    virtual void layoutItemRemoved( QskLayoutItem*, int index );

    virtual QRectF alignedLayoutRect( const QRectF& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline bool QskLayout::isEmpty() const
{
    return itemCount() <= 0;
}

inline void QskLayout::removeItem( const QQuickItem* item )
{
    removeItem( const_cast< QQuickItem* >( item ) );
}

#endif
