/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_BOX_H
#define QSK_LAYOUT_BOX_H

#include "QskBox.h"

class QskLayoutEngine;
class QskLayoutItem;

class QSK_EXPORT QskLayoutBox : public QskBox
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    explicit QskLayoutBox( QQuickItem* parent = 0 );
    ~QskLayoutBox() override;

    Q_INVOKABLE bool isEmpty() const;

    Q_INVOKABLE int itemCount() const;
    Q_INVOKABLE QQuickItem* itemAtIndex( int index ) const;
    Q_INVOKABLE int indexOf( const QQuickItem* ) const;

    void removeItem( const QQuickItem* );
    Q_INVOKABLE void removeItem( QQuickItem* );
    Q_INVOKABLE void removeAt( int index );

    Q_INVOKABLE void clear( bool autoDelete = false );

    Q_INVOKABLE void setActive( bool );
    Q_INVOKABLE bool isActive() const;

    void adjustItem( const QQuickItem* );
    void adjustItemAt( int index );

    QSizeF contentsSizeHint() const override;

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

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
    virtual QSizeF layoutItemsSizeHint() const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline bool QskLayoutBox::isEmpty() const
{
    return itemCount() <= 0;
}

inline void QskLayoutBox::removeItem( const QQuickItem* item )
{
    removeItem( const_cast< QQuickItem* >( item ) );
}

#endif
