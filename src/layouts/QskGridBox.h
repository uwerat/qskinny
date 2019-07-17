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

    Q_PROPERTY( bool empty READ isEmpty() )
    Q_PROPERTY( int count READ count )

    using Inherited = QskBox;

  public:
    explicit QskGridBox( QQuickItem* parent = nullptr );
    ~QskGridBox() override;

    Q_INVOKABLE void addItem(
        QQuickItem*, int row, int column, int rowSpan, int columnSpan,
        Qt::Alignment alignment = Qt::Alignment() );

    Q_INVOKABLE void addItem(
        QQuickItem*, int row, int column,
        Qt::Alignment alignment = Qt::Alignment() );

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

    Q_INVOKABLE int rowOfIndex( int index ) const;
    Q_INVOKABLE int rowSpanOfIndex( int index ) const;

    Q_INVOKABLE int columnOfIndex( int index ) const;
    Q_INVOKABLE int columnSpanOfIndex( int index ) const;

    // spacings

    void setSpacing( Qt::Orientations, qreal spacing );
    void resetSpacing( Qt::Orientations );
    qreal spacing( Qt::Orientation ) const;

    void setSpacing( qreal spacing );

#ifdef QSK_LAYOUT_COMPAT

    void setVerticalSpacing( qreal spacing ) { setSpacing( Qt::Vertical, spacing ); }
    qreal verticalSpacing() const { return spacing( Qt::Vertical ); }

    void setHorizontalSpacing( qreal spacing ) { setSpacing( Qt::Horizontal, spacing ); }
    qreal horizontalSpacing() const { return spacing( Qt::Horizontal ); }

    void setSpacing( qreal spacing ) { setSpacing( Qt::Horizontal | Qt::Vertical, spacing ); }

    void setActive( bool ) {}
    bool isActive() const { return true; }

    void setRowMinimumHeight( int column, qreal height )
        { setRowSizeHint( column, Qt::MinimumSize, height ); }

    void setColumnMaximumWidth( int column, qreal width )
        { setColumnSizeHint( column, Qt::MaximumSize, width ); }
#endif

    Q_INVOKABLE void setRowSpacing( int row, qreal spacing );
    Q_INVOKABLE qreal rowSpacing( int row ) const;

    Q_INVOKABLE void setColumnSpacing( int column, qreal spacing );
    Q_INVOKABLE qreal columnSpacing( int column ) const;

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

    // alignments

    Q_INVOKABLE void setRowAlignment( int row, Qt::Alignment alignment );
    Q_INVOKABLE Qt::Alignment rowAlignment( int row ) const;

    Q_INVOKABLE void setColumnAlignment( int column, Qt::Alignment alignment );
    Q_INVOKABLE Qt::Alignment columnAlignment( int column ) const;

    void setAlignment( const QQuickItem* item, Qt::Alignment alignment );
    Qt::Alignment alignment( const QQuickItem* item ) const;

    bool retainSizeWhenHidden( const QQuickItem* ) const;
    void setRetainSizeWhenHidden( const QQuickItem*, bool on );

    QSizeF contentsSizeHint() const override;

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

  public Q_SLOTS:
    void invalidate();
    void clear( bool autoDelete = false );

  protected:
    bool event( QEvent* ) override;
    void geometryChangeEvent( QskGeometryChangeEvent* ) override;

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void updateLayout() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline void QskGridBox::addItem(
    QQuickItem* item, int row, int column, Qt::Alignment alignment )
{
    addItem( item, row, column, 1, 1, alignment );
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
