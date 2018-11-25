/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_ITEM_H
#define QSK_LAYOUT_ITEM_H

#include "QskGlobal.h"

QSK_QT_PRIVATE_BEGIN
/*
    QskLayoutItem.h should be hidden into some cpp file
    as it needs private headers. TODO
 */

#include <private/qgridlayoutengine_p.h>
QSK_QT_PRIVATE_END

class QQuickItem;

class QskLayoutItem : public QGridLayoutItem
{
    using Inherited = QGridLayoutItem;

  public:
    enum UpdateMode
    {
        UpdateNone,
        UpdateWhenVisible,
        UpdateAlways
    };

    QskLayoutItem( QQuickItem* item, int row, int column,
        int rowSpan = 1, int columnSpan = 1 );

    QskLayoutItem( const QSizeF& size, int stretch, int row, int column );

    ~QskLayoutItem() override;

    QQuickItem* item();
    const QQuickItem* item() const;

    QLayoutPolicy::Policy sizePolicy( Qt::Orientation ) const override final;
    QSizeF sizeHint( Qt::SizeHint, const QSizeF& ) const override final;
    void setGeometry( const QRectF& ) override final;

    bool hasDynamicConstraint() const override final;
    Qt::Orientation dynamicConstraintOrientation() const override final;

    bool isIgnored() const override final;
    QLayoutPolicy::ControlTypes controlTypes( LayoutSide side ) const override final;

    bool retainSizeWhenHidden() const;
    void setRetainSizeWhenHidden( bool on );

    UpdateMode updateMode() const;
    void setUpdateMode( UpdateMode );

    QSizeF spacingHint() const;
    void setSpacingHint( const QSizeF& );

    bool isGeometryDirty() const;

    bool hasUnlimitedSpan() const;
    bool hasUnlimitedSpan( Qt::Orientation orientation ) const;

  private:
    QQuickItem* m_item;
    QSizeF m_spacingHint;

    bool m_isGeometryDirty : 1;
    bool m_isStretchable : 1;
    bool m_retainSizeWhenHidden : 1;
    bool m_unlimitedRowSpan : 1;
    bool m_unlimitedColumnSpan : 1;
    UpdateMode m_updateMode : 2;
};

inline QQuickItem* QskLayoutItem::item()
{
    return m_item;
}

inline const QQuickItem* QskLayoutItem::item() const
{
    return m_item;
}

inline bool QskLayoutItem::isGeometryDirty() const
{
    return m_isGeometryDirty;
}

inline bool QskLayoutItem::hasUnlimitedSpan( Qt::Orientation orientation ) const
{
    return ( orientation == Qt::Horizontal )
           ? m_unlimitedColumnSpan : m_unlimitedRowSpan;
}

inline bool QskLayoutItem::hasUnlimitedSpan() const
{
    return m_unlimitedColumnSpan || m_unlimitedRowSpan;
}

#endif
