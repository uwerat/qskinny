/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRID_BOX_H
#define QSK_GRID_BOX_H

#include "QskBox.h"

class QSK_EXPORT QskGridBox : public QskBox
{
    Q_OBJECT

    Q_PROPERTY( Qt::Alignment defaultAlignment READ defaultAlignment
        WRITE setDefaultAlignment NOTIFY defaultAlignmentChanged )

    Q_PROPERTY( bool empty READ isEmpty() )
    Q_PROPERTY( int elementCount READ elementCount )

    using Inherited = QskBox;

  public:
    explicit QskGridBox( QQuickItem* parent = nullptr );
    ~QskGridBox() override;

    Q_INVOKABLE int addItem( QQuickItem*,
        int row, int column, int rowSpan, int columnSpan );

    Q_INVOKABLE int addItem( QQuickItem*, int row, int column );

    int addItem( QQuickItem*, int row, int column, Qt::Alignment );

    int addItem( QQuickItem*, int row, int column,
        int rowSpan, int columnSpan, Qt::Alignment );

    Q_INVOKABLE int addSpacer( const QSizeF&,
        int row, int column, int rowSpan = 1, int columnSpan = 1 );

    int addColumnSpacer( qreal spacing, int column );
    int addRowSpacer( qreal spacing, int row );

    void removeItem( const QQuickItem* );
    void removeAt( int index );

    Q_INVOKABLE int rowCount() const;
    Q_INVOKABLE int columnCount() const;

    int elementCount() const;

    QQuickItem* itemAtIndex( int index ) const;
    int indexOf( const QQuickItem* ) const;

    bool isEmpty() const;

    Q_INVOKABLE QQuickItem* itemAt( int row, int column ) const;
    Q_INVOKABLE int indexAt( int row, int column ) const;

    Q_INVOKABLE QRect gridOfIndex( int index ) const;
    Q_INVOKABLE QRect effectiveGridOfIndex( int index ) const;

    void setDefaultAlignment( Qt::Alignment );
    Qt::Alignment defaultAlignment() const;

    void setSpacing( Qt::Orientations, qreal spacing );
    void resetSpacing( Qt::Orientations );
    qreal spacing( Qt::Orientation ) const;

    void setSpacing( qreal spacing );

    // stretch factors
    Q_INVOKABLE void setRowStretchFactor( int row, int stretch );
    Q_INVOKABLE int rowStretchFactor( int row ) const;

    Q_INVOKABLE void setColumnStretchFactor( int column, int stretch );
    Q_INVOKABLE int columnStretchFactor( int column ) const;

    // row/column size hints

    Q_INVOKABLE void setColumnWidthHint( int column, Qt::SizeHint, qreal width );
    Q_INVOKABLE qreal columnWidthHint( int column, Qt::SizeHint ) const;

    Q_INVOKABLE void setRowHeightHint( int row, Qt::SizeHint, qreal height );
    Q_INVOKABLE qreal rowHeightHint( int row, Qt::SizeHint ) const;

    Q_INVOKABLE void setRowFixedHeight( int row, qreal height );
    Q_INVOKABLE void setColumnFixedWidth( int column, qreal width );

    void dump() const;

  public Q_SLOTS:
    void invalidate();
    void clear( bool autoDelete = false );

  Q_SIGNALS:
    void defaultAlignmentChanged();

  protected:
    bool event( QEvent* ) override;
    void geometryChangeEvent( QskGeometryChangeEvent* ) override;

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void updateLayout() override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    void setItemActive( QQuickItem*, bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline int QskGridBox::addItem( QQuickItem* item, int row, int column )
{
    return addItem( item, row, column, 1, 1 );
}

inline bool QskGridBox::isEmpty() const
{
    return elementCount() <= 0;
}

inline void QskGridBox::setSpacing( qreal spacing )
{
    setSpacing( Qt::Horizontal | Qt::Vertical, spacing );
}

#endif
