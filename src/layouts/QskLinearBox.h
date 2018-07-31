/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LINEAR_BOX_H
#define QSK_LINEAR_BOX_H

#include "QskGlobal.h"
#include "QskIndexedLayoutBox.h"

class QSK_EXPORT QskLinearBox : public QskIndexedLayoutBox
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged FINAL )

    Q_PROPERTY( uint dimension READ dimension
        WRITE setDimension NOTIFY dimensionChanged FINAL )

    Q_PROPERTY( qreal spacing READ spacing
        WRITE setSpacing RESET resetSpacing
        NOTIFY spacingChanged FINAL )

    Q_PROPERTY( Qt::Edges extraSpacingAt READ extraSpacingAt
        WRITE setExtraSpacingAt NOTIFY extraSpacingAtChanged )

    using Inherited = QskIndexedLayoutBox;

public:
    explicit QskLinearBox( QQuickItem* parent = nullptr );
    explicit QskLinearBox( Qt::Orientation, QQuickItem* parent = nullptr );

    QskLinearBox( Qt::Orientation, uint dimension, QQuickItem* parent = nullptr );

    ~QskLinearBox() override;

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation );

    void setDimension( uint );
    uint dimension() const;

    void setExtraSpacingAt( Qt::Edges );
    Qt::Edges extraSpacingAt() const;

    void setSpacing( qreal spacing );
    void resetSpacing();
    qreal spacing() const;

    Q_INVOKABLE void addSpacer( qreal spacing, int stretchFactor = 0 );
    Q_INVOKABLE void insertSpacer( int index, qreal spacing, int stretchFactor = 0 );

    Q_INVOKABLE void addStretch( int stretchFactor = 0 );
    Q_INVOKABLE void insertStretch( int index, int stretchFactor = 0 );

    Q_INVOKABLE void setStretchFactor( int index, int stretchFactor );
    Q_INVOKABLE int stretchFactor( int index ) const;

    void setStretchFactor( const QQuickItem*, int stretchFactor );
    int stretchFactor( const QQuickItem* ) const;

    Q_INVOKABLE void setStretchFactor( QQuickItem*, int stretchFactor );
    Q_INVOKABLE int stretchFactor( QQuickItem* ) const;

    Q_INVOKABLE bool retainSizeWhenHidden( int index ) const;
    Q_INVOKABLE void setRetainSizeWhenHidden( int index, bool on );

    bool retainSizeWhenHidden( const QQuickItem* ) const;
    void setRetainSizeWhenHidden( const QQuickItem*, bool on );

    Q_INVOKABLE bool retainSizeWhenHidden( QQuickItem* ) const;
    Q_INVOKABLE void setRetainSizeWhenHidden( QQuickItem*, bool on );

    QSizeF contentsSizeHint() const override;

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

#if 1
    Q_INVOKABLE void setRowSpacing( int row, qreal spacing );
    Q_INVOKABLE qreal rowSpacing( int row ) const;

    Q_INVOKABLE void setColumnSpacing( int column, qreal spacing );
    Q_INVOKABLE qreal columnSpacing( int column ) const;

    Q_INVOKABLE void setRowStretchFactor( int row, int stretchFactor );
    Q_INVOKABLE int rowStretchFactor( int row ) const;

    Q_INVOKABLE void setColumnStretchFactor( int column, int stretchFactor );
    Q_INVOKABLE int columnStretchFactor( int column ) const;
#endif

public Q_SLOTS:
    void transpose();

Q_SIGNALS:
    void orientationChanged();
    void dimensionChanged();
    void spacingChanged();
    void extraSpacingAtChanged();

protected:
    QRectF alignedLayoutRect( const QRectF& ) const override;

private:
    void setupLayoutItem( QskLayoutItem*, int index ) override;
    void layoutItemInserted( QskLayoutItem*, int index ) override;
    void layoutItemRemoved( QskLayoutItem*, int index ) override;

    void rearrange();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

// using const is the right thing, unfortunately Qml does not like it.

inline void QskLinearBox::setStretchFactor( const QQuickItem* item, int stretchFactor )
{
    setStretchFactor( const_cast< QQuickItem* >( item ), stretchFactor );
}

inline int QskLinearBox::stretchFactor( const QQuickItem* item ) const
{
    return stretchFactor( const_cast< QQuickItem* >( item ) );
}

inline void QskLinearBox::setRetainSizeWhenHidden( const QQuickItem* item, bool on )
{
    setRetainSizeWhenHidden( const_cast< QQuickItem* >( item ), on );
}

inline bool QskLinearBox::retainSizeWhenHidden( const QQuickItem* item ) const
{
    return retainSizeWhenHidden( const_cast< QQuickItem* >( item ) );
}

#endif
