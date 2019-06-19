/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGridLayoutEngine.h"
#include "QskLayoutConstraint.h"
#include "QskSizePolicy.h"
#include "QskControl.h"
#include "QskQuick.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qgridlayoutengine_p.h>
QSK_QT_PRIVATE_END

namespace
{
    class LayoutStyleInfo final : public QAbstractLayoutStyleInfo
    {
      public:
        qreal spacing( Qt::Orientation ) const override
        {
            // later from the theme !!
            return 5.0;
        }

        qreal windowMargin( Qt::Orientation ) const override
        {
            // later from the theme !!
            return 0;
        }

        bool hasChangedCore() const override
        {
            return false; // never changes
        }
    };

    class LayoutItem final : public QGridLayoutItem
    {
      public:
        LayoutItem( QQuickItem* item, int row, int column,
                int rowSpan, int columnSpan, Qt::Alignment alignment )
            : QGridLayoutItem( row, column,
                qMax( rowSpan, 1 ), qMax( columnSpan, 1 ), alignment )
            , m_item( item )
            , m_retainSizeWhenHidden( false )
            , m_unlimitedRowSpan( rowSpan <= 0 )
            , m_unlimitedColumnSpan( columnSpan <= 0 )
        {
        }

        QQuickItem* item() const
        {
            return m_item;
        }

        void setGeometry( const QRectF& rect ) override
        {
            qskSetItemGeometry( m_item, rect );
        }

        QLayoutPolicy::Policy sizePolicy( Qt::Orientation orientation ) const override
        {
            const auto policy = QskLayoutConstraint::sizePolicy( m_item );
            return static_cast< QLayoutPolicy::Policy >( policy.policy( orientation ) );
        }

        QSizeF sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const override
        {
            return QskLayoutConstraint::sizeHint( m_item, which, constraint );
        }

        bool hasDynamicConstraint() const override
        {
            using namespace QskLayoutConstraint;
            return constraintType( m_item ) != Unconstrained;
        }

        Qt::Orientation dynamicConstraintOrientation() const override
        {
            Qt::Orientation orientation = Qt::Vertical;

            if ( auto control = qskControlCast( m_item ) )
            {
                const auto policy = control->sizePolicy().horizontalPolicy();

                return ( policy == QskSizePolicy::Constrained )
                    ? Qt::Horizontal : Qt::Vertical;
            }

            return orientation;
        }

        bool isIgnored() const override final
        {
            if ( m_item && !qskIsVisibleToParent( m_item ) )
                return !m_retainSizeWhenHidden;

            return false;
        }

        QLayoutPolicy::ControlTypes controlTypes( LayoutSide ) const override
        {
            return QLayoutPolicy::DefaultType;
        }

        bool retainSizeWhenHidden() const
        {
            return m_retainSizeWhenHidden;
        }

        void setRetainSizeWhenHidden( bool on )
        {
            m_retainSizeWhenHidden = on;
        }

        bool hasUnlimitedSpan() const
        {
            return m_unlimitedColumnSpan || m_unlimitedRowSpan;
        }

        bool hasUnlimitedSpan( Qt::Orientation orientation ) const
        {
            return ( orientation == Qt::Horizontal )
                   ? m_unlimitedColumnSpan : m_unlimitedRowSpan;
        }

      private:
        QQuickItem* m_item;

        bool m_retainSizeWhenHidden : 1;
        bool m_unlimitedRowSpan : 1;
        bool m_unlimitedColumnSpan : 1;
    };

    class LayoutEngine : public QGridLayoutEngine
    {
      public:
        LayoutEngine()
            : QGridLayoutEngine( Qt::AlignVCenter, false /*snapToPixelGrid*/ )
        {
            /*
                snapToPixelGrid rounds x/y, what might lead to losing a pixel.
                F.e. when having a text in elideMode we end up with an elided text
                because of this.
             */
        }

        LayoutItem* layoutItemAt( int index ) const
        {
            if ( index < 0 || index >= q_items.count() )
                return nullptr;

            return static_cast< LayoutItem* >( q_items[ index ] );
        }

        LayoutItem* layoutItemAt( int row, int column ) const
        {
            if ( row < 0 || row >= rowCount() || column < 0 || column >= columnCount() )
                return nullptr;

            return static_cast< LayoutItem* >( itemAt( row, column ) );
        }

        inline LayoutItem* layoutItemOf( const QQuickItem* item ) const
        {
            return layoutItemAt( indexOf( item ) );
        }

        int indexAt( int row, int column ) const
        {
            const auto item = layoutItemAt( row, column );
            if ( item )
                return q_items.indexOf( item );

            return -1;
        }

        int indexOf( const QQuickItem* item ) const
        {
            // linear search might become slow for many items,
            // better introduce some sort of hash table TODO ...

            for ( int i = q_items.count() - 1; i >= 0; --i )
            {
                const auto layoutItem = static_cast< const LayoutItem* >( q_items[ i ] );
                if ( layoutItem->item() == item )
                    return i;
            }

            return -1;
        }

        qreal spacing( Qt::Orientation orientation ) const
        {
            const LayoutStyleInfo styleInfo;
            return QGridLayoutEngine::spacing( orientation, &styleInfo );
        }
    };
}

class QskGridLayoutEngine::PrivateData
{
  public:
    /*
        For the moment we use QGridLayoutEngine, but sooner and later
        it should be replaced by a new implementation, that uses
        the same backend as QskLinearLayoutEngine.
     */
    LayoutEngine qengine;
    unsigned int unlimitedSpanned = 0;
};


QskGridLayoutEngine::QskGridLayoutEngine()
    : m_data( new PrivateData() )
{
}

QskGridLayoutEngine::~QskGridLayoutEngine()
{
}

void QskGridLayoutEngine::setGeometries( const QRectF rect )
{
    const LayoutStyleInfo styleInfo;
    m_data->qengine.setGeometries( rect, &styleInfo );
}

void QskGridLayoutEngine::invalidate()
{
    m_data->qengine.invalidate();
}

void QskGridLayoutEngine::setVisualDirection( Qt::LayoutDirection direction )
{
    m_data->qengine.setVisualDirection( direction );
}

Qt::LayoutDirection QskGridLayoutEngine::visualDirection() const
{
    return m_data->qengine.visualDirection();
}

int QskGridLayoutEngine::itemCount() const
{
    return m_data->qengine.itemCount();
}

int QskGridLayoutEngine::rowCount() const
{
    return m_data->qengine.rowCount();
}

int QskGridLayoutEngine::columnCount() const
{
    return m_data->qengine.columnCount();
}

void QskGridLayoutEngine::insertItem( QQuickItem* item,
    int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment )
{
    auto& qengine = m_data->qengine;

    auto layoutItem = new LayoutItem(
        item, row, column, rowSpan, columnSpan, alignment );

    const bool isExpanding = ( layoutItem->lastColumn() >= qengine.columnCount() ) ||
        ( layoutItem->lastRow() >= qengine.rowCount() );

    qengine.insertItem( layoutItem, -1 );

    if ( isExpanding )
    {
        // the new item has extended the number of rows/columns and
        // we need to adjust all items without fixed spanning

        if ( m_data->unlimitedSpanned > 0 )
            adjustSpans( columnCount(), rowCount() );
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

void QskGridLayoutEngine::removeAt( int index )
{
    auto& qengine = m_data->qengine;

    auto layoutItem = qengine.layoutItemAt( index );
    if ( layoutItem == nullptr )
        return;

    qengine.removeItem( layoutItem );

    if ( layoutItem->hasUnlimitedSpan() )
        m_data->unlimitedSpanned--;

    // cleanup rows/columns

    const QSize cells = requiredCells();

    const int numPendingColumns = qengine.columnCount() - cells.width();
    const int numPendingRows = qengine.rowCount() - cells.height();

    if ( numPendingColumns > 0 || numPendingRows > 0 )
    {
        if ( m_data->unlimitedSpanned > 0 )
            adjustSpans( cells.height(), cells.width() );

        qengine.removeRows( cells.width(), numPendingColumns, Qt::Horizontal );
        qengine.removeRows( cells.height(), numPendingRows, Qt::Vertical );
    }

    delete layoutItem;
}

QQuickItem* QskGridLayoutEngine::itemAt( int index ) const
{
    if ( const auto layoutItem = m_data->qengine.layoutItemAt( index ) )
        return layoutItem->item();

    return nullptr;
}

QQuickItem* QskGridLayoutEngine::itemAt( int row, int column ) const
{
    if ( const auto layoutItem = m_data->qengine.layoutItemAt( row, column ) )
        return layoutItem->item();

    return nullptr;
}

int QskGridLayoutEngine::indexAt( int row, int column ) const
{
    return m_data->qengine.indexAt( row, column );
}

int QskGridLayoutEngine::indexOf( const QQuickItem* item ) const
{
    if ( item == nullptr )
        return -1;

    return m_data->qengine.indexOf( item );
}

int QskGridLayoutEngine::rowOfIndex( int index ) const
{
    if ( auto layoutItem = m_data->qengine.layoutItemAt( index ) )
        return layoutItem->firstRow();

    return -1;
}

int QskGridLayoutEngine::rowSpanOfIndex( int index ) const
{
    if ( auto layoutItem = m_data->qengine.layoutItemAt( index ) )
        return layoutItem->rowSpan();

    return 0;
}

int QskGridLayoutEngine::columnOfIndex( int index ) const
{
    if ( auto layoutItem = m_data->qengine.layoutItemAt( index ) )
        return layoutItem->firstColumn();

    return -1;
}

int QskGridLayoutEngine::columnSpanOfIndex( int index ) const
{
    if ( auto layoutItem = m_data->qengine.layoutItemAt( index ) )
        return layoutItem->columnSpan();

    return 0;
}

void QskGridLayoutEngine::setSpacing(
    Qt::Orientation orientation, qreal spacing )
{
    m_data->qengine.setSpacing( spacing, orientation );
}

qreal QskGridLayoutEngine::spacing( Qt::Orientation orientation ) const
{
    return m_data->qengine.spacing( orientation );
}

void QskGridLayoutEngine::setSpacingAt(
    Qt::Orientation orientation, int cell, qreal spacing )
{
    // is this a spacer ???
    m_data->qengine.setRowSpacing( cell, spacing, orientation );
}

qreal QskGridLayoutEngine::spacingAt(
    Qt::Orientation orientation, int cell ) const
{
    return m_data->qengine.rowSpacing( cell, orientation );
}

void QskGridLayoutEngine::setStretchFactorAt(
    Qt::Orientation orientation, int cell, int stretch )
{
    m_data->qengine.setRowStretchFactor( cell, stretch, orientation );
}

int QskGridLayoutEngine::stretchFactorAt( Qt::Orientation orientation, int cell )
{
    return m_data->qengine.rowStretchFactor( cell, orientation );
}

void QskGridLayoutEngine::setAlignmentAt(
    Qt::Orientation orientation, int cell, Qt::Alignment alignment )
{
    m_data->qengine.setRowAlignment( cell, alignment, orientation );
}

Qt::Alignment QskGridLayoutEngine::alignmentAt(
    Qt::Orientation orientation, int cell ) const
{
    return m_data->qengine.rowAlignment( cell, orientation );
}

void QskGridLayoutEngine::setAlignmentOf(
    const QQuickItem* item, Qt::Alignment alignment )
{
    if ( auto layoutItem = m_data->qengine.layoutItemOf( item ) )
        layoutItem->setAlignment( alignment );
}

Qt::Alignment QskGridLayoutEngine::alignmentOf( const QQuickItem* item ) const
{
    if ( const auto layoutItem = m_data->qengine.layoutItemOf( item ) )
        return layoutItem->alignment();

    return Qt::Alignment();
}

void QskGridLayoutEngine::setRetainSizeWhenHiddenOf( const QQuickItem* item, bool on )
{
    if ( auto layoutItem = m_data->qengine.layoutItemOf( item ) )
        layoutItem->setRetainSizeWhenHidden( on );
}

bool QskGridLayoutEngine::retainSizeWhenHiddenOf( const QQuickItem* item ) const
{
    if ( const auto layoutItem = m_data->qengine.layoutItemOf( item ) )
        return layoutItem->retainSizeWhenHidden();

    return false;
}

void QskGridLayoutEngine::setRowSizeHint( int row, Qt::SizeHint which, qreal height )
{
    m_data->qengine.setRowSizeHint( which, row, height, Qt::Vertical );
}

qreal QskGridLayoutEngine::rowSizeHint( int row, Qt::SizeHint which ) const
{
    return m_data->qengine.rowSizeHint( which, row, Qt::Vertical );
}

void QskGridLayoutEngine::setColumnSizeHint( int column, Qt::SizeHint which, qreal width )
{
    m_data->qengine.setRowSizeHint( which, column, width, Qt::Horizontal );
}

qreal QskGridLayoutEngine::columnSizeHint( int column, Qt::SizeHint which ) const
{
    return m_data->qengine.rowSizeHint( which, column, Qt::Horizontal );
}

QSizeF QskGridLayoutEngine::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    const LayoutStyleInfo styleInfo;
    return m_data->qengine.sizeHint( which, constraint, &styleInfo );
}

qreal QskGridLayoutEngine::widthForHeight( qreal height ) const
{
    const QSizeF constraint( -1, height );
    return sizeHint( Qt::PreferredSize, constraint ).width();
}

qreal QskGridLayoutEngine::heightForWidth( qreal width ) const
{
    const QSizeF constraint( width, -1 );
    return sizeHint( Qt::PreferredSize, constraint ).height();
}

qreal QskGridLayoutEngine::defaultSpacing( Qt::Orientation orientation )
{
    return LayoutStyleInfo().spacing( orientation );
}

QSize QskGridLayoutEngine::requiredCells() const
{
    int lastRow = -1;
    int lastColumn = -1;

    for ( int i = 0; i < m_data->qengine.itemCount(); i++ )
    {
        const auto layoutItem = m_data->qengine.layoutItemAt( i );
        if ( layoutItem->isIgnored() )
            continue;

        const int col = layoutItem->hasUnlimitedSpan( Qt::Horizontal )
            ? layoutItem->firstColumn() + 1 : layoutItem->lastColumn();

        if ( col > lastColumn )
            lastColumn = col;

        const int row = layoutItem->hasUnlimitedSpan( Qt::Vertical )
            ? layoutItem->firstRow() + 1 : layoutItem->lastRow();

        if ( row > lastRow )
            lastRow = row;
    }

    return QSize( lastColumn + 1, lastRow + 1 );
}

void QskGridLayoutEngine::adjustSpans( int numRows, int numColumns )
{
    for ( int i = 0; i < m_data->qengine.itemCount(); i++ )
    {
        auto layoutItem = m_data->qengine.layoutItemAt( i );

        if ( layoutItem->hasUnlimitedSpan( Qt::Horizontal ) )
            layoutItem->setRowSpan( numColumns - layoutItem->firstColumn(), Qt::Horizontal );

        if ( layoutItem->hasUnlimitedSpan( Qt::Vertical ) )
            layoutItem->setRowSpan( numRows - layoutItem->firstRow(), Qt::Vertical );
    }
}
