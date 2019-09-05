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
    Q_PROPERTY( int count READ count )

    using Inherited = QskBox;

  public:
    explicit QskGridBox( QQuickItem* parent = nullptr );
    ~QskGridBox() override;

    Q_INVOKABLE int addItem( QQuickItem*,
        int row, int column, int rowSpan, int columnSpan );

    Q_INVOKABLE int addItem( QQuickItem*, int row, int column );

    Q_INVOKABLE int addSpacer( qreal spacing,
        int row, int column, int columnSpan = 1, int rowSpan = 1 );

    void removeItem( const QQuickItem* );
    void removeAt( int index );

    Q_INVOKABLE int rowCount() const;
    Q_INVOKABLE int columnCount() const;

    int count() const;

#ifdef QSK_LAYOUT_COMPAT
    int itemCount() const { return count(); } // items and spacers
#endif
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

#ifdef QSK_LAYOUT_COMPAT

    void setVerticalSpacing( qreal spacing ) { setSpacing( Qt::Vertical, spacing ); }
    qreal verticalSpacing() const { return spacing( Qt::Vertical ); }

    void setHorizontalSpacing( qreal spacing ) { setSpacing( Qt::Horizontal, spacing ); }
    qreal horizontalSpacing() const { return spacing( Qt::Horizontal ); }

    void setActive( bool ) {}
    bool isActive() const { return true; }

    void setRowMinimumHeight( int row, qreal height )
        { setRowSizeHint( row, Qt::MinimumSize, height ); }

    void setColumnMaximumWidth( int column, qreal width )
        { setColumnSizeHint( column, Qt::MaximumSize, width ); }
#endif

    // stretch factors
    Q_INVOKABLE void setRowStretchFactor( int row, int stretch );
    Q_INVOKABLE int rowStretchFactor( int row ) const;

    Q_INVOKABLE void setColumnStretchFactor( int column, int stretch );
    Q_INVOKABLE int columnStretchFactor( int column ) const;

    // row/column size hints

    Q_INVOKABLE void setColumnSizeHint( int column, Qt::SizeHint, qreal width );
    Q_INVOKABLE qreal columnSizeHint( int column, Qt::SizeHint ) const;

    Q_INVOKABLE void setRowSizeHint( int row, Qt::SizeHint, qreal height );
    Q_INVOKABLE qreal rowSizeHint( int row, Qt::SizeHint ) const;

    Q_INVOKABLE void setRowFixedHeight( int row, qreal height );
    Q_INVOKABLE void setColumnFixedWidth( int column, qreal width );

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

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

    QSizeF contentsSizeHint() const override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline int QskGridBox::addItem( QQuickItem* item, int row, int column )
{
    return addItem( item, row, column, 1, 1 );
}

inline bool QskGridBox::isEmpty() const
{
    return count() <= 0;
}

inline void QskGridBox::setSpacing( qreal spacing )
{
    setSpacing( Qt::Horizontal | Qt::Vertical, spacing );
}

#endif
