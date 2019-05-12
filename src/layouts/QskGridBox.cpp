/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGridBox.h"
#include "QskLayoutEngine.h"
#include "QskLayoutItem.h"

class QskGridBox::PrivateData
{
  public:
    PrivateData()
        : isExpanding( false )
        , unlimitedSpanned( 0 )
    {
    }

    bool isExpanding;
    unsigned int unlimitedSpanned;
};

QskGridBox::QskGridBox( QQuickItem* parent )
    : QskLayoutBox( parent )
    , m_data( new PrivateData() )
{
}

QskGridBox::~QskGridBox()
{
}

void QskGridBox::addItem( QQuickItem* item,
    int row, int column, int rowSpan, int columnSpan,
    Qt::Alignment alignment )
{
    auto layoutItem = new QskLayoutItem( item, row, column, rowSpan, columnSpan );
    layoutItem->setAlignment( alignment );

    const int index = itemCount(); // position and index doesn't match

    setupLayoutItem( layoutItem, index );
    insertItemInternal( layoutItem, -1 );
    layoutItemInserted( layoutItem, index );
}

int QskGridBox::rowCount() const
{
    return engine().rowCount();
}

int QskGridBox::columnCount() const
{
    return engine().columnCount();
}

QQuickItem* QskGridBox::itemAt( int row, int column ) const
{
    if ( const auto layoutItem = engine().layoutItemAt( row, column ) )
        return layoutItem->item();

    return nullptr;
}

int QskGridBox::indexAt( int row, int column ) const
{
    return engine().indexAt( row, column );
}

int QskGridBox::rowOfIndex( int index ) const
{
    if ( auto layoutItem = engine().layoutItemAt( index ) )
        return layoutItem->firstRow();

    return -1;
}

int QskGridBox::rowSpanOfIndex( int index ) const
{
    if ( auto layoutItem = engine().layoutItemAt( index ) )
        return layoutItem->rowSpan();

    return 0;
}

int QskGridBox::columnOfIndex( int index ) const
{
    if ( auto layoutItem = engine().layoutItemAt( index ) )
        return layoutItem->firstColumn();

    return -1;
}

int QskGridBox::columnSpanOfIndex( int index ) const
{
    if ( auto layoutItem = engine().layoutItemAt( index ) )
        return layoutItem->columnSpan();

    return 0;
}

void QskGridBox::setupLayoutItem( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( index )

    auto& engine = this->engine();

    m_data->isExpanding = ( layoutItem->lastColumn() >= engine.columnCount() ) ||
        ( layoutItem->lastRow() >= engine.rowCount() );
}

void QskGridBox::layoutItemInserted( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( index )

    if ( m_data->isExpanding )
    {
        // the new item has extended the number of rows/columns and
        // we need to adjust all items without fixed spanning

        if ( m_data->unlimitedSpanned > 0 )
            engine().adjustSpans( columnCount(), rowCount() );
    }

    if ( layoutItem->hasUnlimitedSpan() )
    {
        // the item itself might need to be adjusted

        if ( layoutItem->hasUnlimitedSpan( Qt::Horizontal ) )
        {
            const int span = columnCount() - layoutItem->firstColumn();
            layoutItem->setRowSpan( span, Qt::Horizontal );
        }

        if ( layoutItem->hasUnlimitedSpan( Qt::Vertical ) )
        {
            const int span = rowCount() - layoutItem->firstRow();
            layoutItem->setRowSpan( span, Qt::Vertical );
        }

        m_data->unlimitedSpanned++;
    }
}

void QskGridBox::layoutItemRemoved( QskLayoutItem* layoutItem, int index )
{
    Q_UNUSED( index )

    if ( layoutItem->hasUnlimitedSpan() )
        m_data->unlimitedSpanned--;

    QskLayoutEngine& engine = this->engine();

    // cleanup rows/columns

    const QSize cells = engine.requiredCells();

    const int numPendingColumns = engine.columnCount() - cells.width();
    const int numPendingRows = engine.rowCount() - cells.height();

    if ( numPendingColumns > 0 || numPendingRows > 0 )
    {
        if ( m_data->unlimitedSpanned > 0 )
            engine.adjustSpans( cells.height(), cells.width() );

        engine.removeRows( cells.width(), numPendingColumns, Qt::Horizontal );
        engine.removeRows( cells.height(), numPendingRows, Qt::Vertical );
    }
}

void QskGridBox::setSpacing( qreal spacing )
{
    setHorizontalSpacing( spacing );
    setVerticalSpacing( spacing );
}

void QskGridBox::setHorizontalSpacing( qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    if ( spacing != engine().spacing( Qt::Horizontal ) )
    {
        engine().setSpacing( spacing, Qt::Horizontal );
        activate();

        Q_EMIT horizontalSpacingChanged();
    }
}

qreal QskGridBox::horizontalSpacing() const
{
    return engine().spacing( Qt::Horizontal );
}

void QskGridBox::resetHorizontalSpacing()
{
    const qreal spacing = QskLayoutEngine::defaultSpacing( Qt::Horizontal );
    setHorizontalSpacing( spacing );
}

void QskGridBox::setVerticalSpacing( qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    if ( spacing != engine().spacing( Qt::Vertical ) )
    {
        engine().setSpacing( spacing, Qt::Vertical );
        activate();

        Q_EMIT verticalSpacingChanged();
    }
}

qreal QskGridBox::verticalSpacing() const
{
    return engine().spacing( Qt::Vertical );
}

void QskGridBox::resetVerticalSpacing()
{
    const qreal spacing = QskLayoutEngine::defaultSpacing( Qt::Vertical );
    setVerticalSpacing( spacing );
}

void QskGridBox::setRowSpacing( int row, qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    if ( spacing != engine().rowSpacing( row, Qt::Vertical ) )
    {
        engine().setRowSpacing( row, spacing, Qt::Vertical );
        activate();
    }
}

qreal QskGridBox::rowSpacing( int row ) const
{
    return engine().rowSpacing( row, Qt::Vertical );
}

void QskGridBox::setColumnSpacing( int column, qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    if ( spacing != engine().rowSpacing( column, Qt::Horizontal ) )
    {
        engine().setRowSpacing( column, spacing, Qt::Horizontal );
        activate();
    }
}

qreal QskGridBox::columnSpacing( int column ) const
{
    return engine().rowSpacing( column, Qt::Horizontal );
}

void QskGridBox::setRowStretchFactor( int row, int stretch )
{
    if ( stretch != engine().rowStretchFactor( row, Qt::Vertical ) )
    {
        engine().setRowStretchFactor( row, stretch, Qt::Vertical );
        activate();
    }
}

int QskGridBox::rowStretchFactor( int row ) const
{
    return engine().rowStretchFactor( row, Qt::Vertical );
}

void QskGridBox::setColumnStretchFactor( int column, int stretch )
{
    if ( stretch != engine().rowStretchFactor( column, Qt::Horizontal ) )
    {
        engine().setRowStretchFactor( column, stretch, Qt::Horizontal );
        activate();
    }
}

int QskGridBox::columnStretchFactor( int column ) const
{
    return engine().rowStretchFactor( column, Qt::Horizontal );
}

void QskGridBox::setRowMinimumHeight( int row, qreal height )
{
    setRowSizeHint( Qt::MinimumSize, row, height, Qt::Vertical );
}

qreal QskGridBox::rowMinimumHeight( int row ) const
{
    return engine().rowSizeHint( Qt::MinimumSize, row, Qt::Vertical );
}

void QskGridBox::setRowPreferredHeight( int row, qreal height )
{
    setRowSizeHint( Qt::PreferredSize, row, height, Qt::Vertical );
}

qreal QskGridBox::rowPreferredHeight( int row ) const
{
    return engine().rowSizeHint( Qt::PreferredSize, row, Qt::Vertical );
}

void QskGridBox::setRowMaximumHeight( int row, qreal height )
{
    setRowSizeHint( Qt::MaximumSize, row, height, Qt::Vertical );
}

qreal QskGridBox::rowMaximumHeight( int row ) const
{
    return engine().rowSizeHint( Qt::MaximumSize, row, Qt::Vertical );
}

void QskGridBox::setRowFixedHeight( int row, qreal height )
{
    setRowMinimumHeight( row, height );
    setRowMaximumHeight( row, height );
}

void QskGridBox::setColumnMinimumWidth( int column, qreal width )
{
    setRowSizeHint( Qt::MinimumSize, column, width, Qt::Horizontal );
}

qreal QskGridBox::columnMinimumWidth( int column ) const
{
    return engine().rowSizeHint( Qt::MinimumSize, column, Qt::Horizontal );
}

void QskGridBox::setColumnPreferredWidth( int column, qreal width )
{
    setRowSizeHint( Qt::PreferredSize, column, width, Qt::Horizontal );
}

qreal QskGridBox::columnPreferredWidth( int column ) const
{
    return engine().rowSizeHint( Qt::PreferredSize, column, Qt::Horizontal );
}

void QskGridBox::setColumnMaximumWidth( int column, qreal width )
{
    setRowSizeHint( Qt::MaximumSize, column, width, Qt::Horizontal );
}

qreal QskGridBox::columnMaximumWidth( int column ) const
{
    return engine().rowSizeHint( Qt::MaximumSize, column, Qt::Horizontal );
}

void QskGridBox::setColumnFixedWidth( int column, qreal width )
{
    setColumnMinimumWidth( column, width );
    setColumnMaximumWidth( column, width );
}

void QskGridBox::setRowAlignment( int row, Qt::Alignment alignment )
{
    if ( engine().rowAlignment( row, Qt::Vertical ) != alignment )
    {
        engine().setRowAlignment( row, alignment, Qt::Vertical );
        activate();
    }
}

Qt::Alignment QskGridBox::rowAlignment( int row ) const
{
    return engine().rowAlignment( row, Qt::Vertical );
}

void QskGridBox::setColumnAlignment( int column, Qt::Alignment alignment )
{
    if ( alignment != engine().rowAlignment( column, Qt::Horizontal ) )
    {
        engine().setRowAlignment( column, alignment, Qt::Horizontal );
        activate();
    }
}

Qt::Alignment QskGridBox::columnAlignment( int column ) const
{
    return engine().rowAlignment( column, Qt::Horizontal );
}

void QskGridBox::setAlignment( const QQuickItem* item, Qt::Alignment alignment )
{
    QskLayoutItem* layoutItem = engine().layoutItemOf( item );
    if ( layoutItem && layoutItem->alignment() != alignment )
    {
        layoutItem->setAlignment( alignment );
        activate();
    }
}

Qt::Alignment QskGridBox::alignment( const QQuickItem* item ) const
{
    QskLayoutItem* layoutItem = engine().layoutItemOf( item );
    if ( layoutItem )
        return layoutItem->alignment();

    return Qt::Alignment();
}

void QskGridBox::setRetainSizeWhenHidden( const QQuickItem* item, bool on )
{
    QskLayoutItem* layoutItem = engine().layoutItemOf( item );
    if ( layoutItem && on != layoutItem->retainSizeWhenHidden() )
    {
        layoutItem->setRetainSizeWhenHidden( on );
        invalidate();
    }
}

bool QskGridBox::retainSizeWhenHidden( const QQuickItem* item ) const
{
    QskLayoutItem* layoutItem = engine().layoutItemOf( item );
    if ( layoutItem )
        return layoutItem->retainSizeWhenHidden();

    return false;
}

void QskGridBox::setRowSizeHint(
    Qt::SizeHint which, int row, qreal size, Qt::Orientation orientation )
{
    if ( size != engine().rowSizeHint( which, row, orientation ) )
    {
        engine().setRowSizeHint( which, row, size, orientation );
        activate();
    }
}

#include "moc_QskGridBox.cpp"
