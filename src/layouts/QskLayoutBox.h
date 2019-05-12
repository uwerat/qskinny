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

    // signals ???
    Q_PROPERTY( int itemCount READ itemCount() )
    Q_PROPERTY( bool empty READ isEmpty() )

    Q_PROPERTY( bool active READ isActive
        WRITE setActive NOTIFY activeChanged )

    using Inherited = QskBox;

  public:
    explicit QskLayoutBox( QQuickItem* parent = nullptr );
    ~QskLayoutBox() override;

    bool isEmpty() const;

    int itemCount() const;
    QQuickItem* itemAtIndex( int index ) const;
    int indexOf( const QQuickItem* ) const;

    void removeItem( const QQuickItem* );
    void removeAt( int index );

    void setActive( bool );
    bool isActive() const;

    void adjustItem( const QQuickItem* );
    void adjustItemAt( int index );

    QSizeF contentsSizeHint() const override;

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

  Q_SIGNALS:
    void activeChanged( bool );

  public Q_SLOTS:
    void activate();
    void invalidate();
    void clear( bool autoDelete = false );

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

#endif
