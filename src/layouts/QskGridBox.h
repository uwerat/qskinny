/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRID_BOX_H
#define QSK_GRID_BOX_H

#include "QskGlobal.h"
#include "QskLayout.h"

class QSK_EXPORT QskGridBox : public QskLayout
{
    Q_OBJECT

    Q_PROPERTY( qreal horizontalSpacing READ horizontalSpacing
        WRITE setHorizontalSpacing RESET resetHorizontalSpacing
        NOTIFY horizontalSpacingChanged )

    Q_PROPERTY( qreal verticalSpacing READ verticalSpacing
        WRITE setVerticalSpacing RESET resetVerticalSpacing
        NOTIFY verticalSpacingChanged )

    using Inherited = QskLayout;

public:
    explicit QskGridBox( QQuickItem* parent = nullptr );
    virtual ~QskGridBox();

    Q_INVOKABLE void addItem(
        QQuickItem*, int row, int column, int rowSpan, int columnSpan,
        Qt::Alignment alignment = Qt::Alignment() );

    Q_INVOKABLE void addItem(
        QQuickItem*, int row, int column,
        Qt::Alignment alignment = Qt::Alignment() );

    Q_INVOKABLE int rowCount() const;
    Q_INVOKABLE int columnCount() const;

    Q_INVOKABLE QQuickItem* itemAt( int row, int column ) const;
    Q_INVOKABLE int indexAt( int row, int column ) const;

    Q_INVOKABLE int rowOfIndex( int index ) const;
    Q_INVOKABLE int rowSpanOfIndex( int index ) const;

    Q_INVOKABLE int columnOfIndex( int index ) const;
    Q_INVOKABLE int columnSpanOfIndex( int index ) const;

    // spacings
    void setSpacing( qreal spacing );

    void setHorizontalSpacing( qreal spacing );
    void resetHorizontalSpacing();
    int horizontalSpacing() const;

    void setVerticalSpacing( qreal spacing );
    void resetVerticalSpacing();
    int verticalSpacing() const;

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
    Q_INVOKABLE void setRowMinimumHeight( int row, qreal height );
    Q_INVOKABLE qreal rowMinimumHeight( int row ) const;

    Q_INVOKABLE void setRowPreferredHeight( int row, qreal height );
    Q_INVOKABLE qreal rowPreferredHeight( int row ) const;

    Q_INVOKABLE void setRowMaximumHeight( int row, qreal height );
    Q_INVOKABLE qreal rowMaximumHeight( int row ) const;

    Q_INVOKABLE void setRowFixedHeight( int row, qreal height );

    Q_INVOKABLE void setColumnMinimumWidth( int column, qreal width );
    Q_INVOKABLE qreal columnMinimumWidth( int column ) const;

    Q_INVOKABLE void setColumnPreferredWidth( int column, qreal width );
    Q_INVOKABLE qreal columnPreferredWidth( int column ) const;

    Q_INVOKABLE void setColumnMaximumWidth( int column, qreal width );
    Q_INVOKABLE qreal columnMaximumWidth( int column ) const;

    Q_INVOKABLE void setColumnFixedWidth( int column, qreal width );

    // alignments

    Q_INVOKABLE void setRowAlignment( int row, Qt::Alignment alignment );
    Q_INVOKABLE Qt::Alignment rowAlignment( int row ) const;

    Q_INVOKABLE void setColumnAlignment( int column, Qt::Alignment alignment );
    Q_INVOKABLE Qt::Alignment columnAlignment( int column ) const;

    void setAlignment( const QQuickItem* item, Qt::Alignment alignment );
    Qt::Alignment alignment( const QQuickItem* item ) const;

    Q_INVOKABLE void setAlignment( QQuickItem* item, Qt::Alignment alignment );
    Q_INVOKABLE Qt::Alignment alignment( QQuickItem* item ) const;

    bool retainSizeWhenHidden( const QQuickItem* ) const;
    void setRetainSizeWhenHidden( const QQuickItem*, bool on );

    Q_INVOKABLE bool retainSizeWhenHidden( QQuickItem* ) const;
    Q_INVOKABLE void setRetainSizeWhenHidden( QQuickItem*, bool on );

    virtual qreal heightForWidth( qreal width ) const override;
    virtual qreal widthForHeight( qreal height ) const override;

    virtual QSizeF contentsSizeHint() const override;

Q_SIGNALS:
    void verticalSpacingChanged();
    void horizontalSpacingChanged();

protected:
    virtual void setupLayoutItem( QskLayoutItem*, int index ) override;
    virtual void layoutItemInserted( QskLayoutItem*, int index ) override;
    virtual void layoutItemRemoved( QskLayoutItem*, int index ) override;

private:
    void setRowSizeHint(
        Qt::SizeHint which, int row, qreal size,
        Qt::Orientation orientation );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline void QskGridBox::addItem(
    QQuickItem* item, int row, int column, Qt::Alignment alignment )
{
    addItem( item, row, column, 1, 1, alignment );
}

// Qml does not like the const versions

inline void QskGridBox::setAlignment( const QQuickItem* item, Qt::Alignment alignment )
{
    setAlignment( const_cast< QQuickItem* >( item ), alignment );
}

inline Qt::Alignment QskGridBox::alignment( const QQuickItem* item ) const
{
    return alignment( const_cast< QQuickItem* >( item ) );
}

inline bool QskGridBox::retainSizeWhenHidden( const QQuickItem* item ) const
{
    return retainSizeWhenHidden( const_cast< QQuickItem* >( item ) );
}

inline void QskGridBox::setRetainSizeWhenHidden( const QQuickItem* item, bool on )
{
    setRetainSizeWhenHidden( const_cast< QQuickItem* >( item ), on );
}

#endif
