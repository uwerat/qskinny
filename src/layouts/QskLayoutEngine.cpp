/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutEngine.h"
#include "QskLayoutItem.h"

static inline bool qskIsColliding(
    const QskLayoutEngine* engine, QskLayoutItem* item )
{
    for ( int row = item->firstRow(); row <= item->lastRow(); row++ )
    {
        for ( int col = item->firstColumn(); col <= item->lastColumn(); col++ )
        {
            if ( engine->itemAt(row, col) )
                return true;
        }
    }

    return false;
}

namespace
{
    class MessageHandler
    {
    public:
        MessageHandler()
        {
            m_defaultHandler = qInstallMessageHandler( suppressWarning );
        }

        ~MessageHandler()
        {
            qInstallMessageHandler( m_defaultHandler );
        }

    private:
        static void suppressWarning( QtMsgType type,
            const QMessageLogContext& context, const QString& message )
        {
            if ( type == QtWarningMsg )
                return;

            if ( m_defaultHandler )
                ( *m_defaultHandler )( type, context, message );
        }

        static QtMessageHandler m_defaultHandler;
    };

    QtMessageHandler MessageHandler::m_defaultHandler;

    class LayoutStyleInfo final : public QAbstractLayoutStyleInfo
    {
    public:
        LayoutStyleInfo()
        {
        }

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
}

QskLayoutEngine::QskLayoutEngine():
    QGridLayoutEngine( Qt::AlignVCenter, true /*snapToPixelGrid*/ )
{
}

QskLayoutEngine::~QskLayoutEngine()
{
}

void QskLayoutEngine::setGeometries( const QRectF rect )
{
    const LayoutStyleInfo styleInfo;
    QGridLayoutEngine::setGeometries( rect, &styleInfo );
}

void QskLayoutEngine::insertLayoutItem( QskLayoutItem* item, int index )
{
    if ( qskIsColliding( this, item ) )
    {
        // It is totally valid to have more than one item in the same cell
        // and we make use of it f.e in QskStackLayout. So we better
        // suppress the corresponding warning to avoid confusion.

        MessageHandler suppressWarning;
        insertItem( item, index );
    }
    else
    {
        insertItem( item, index );
    }
}

int QskLayoutEngine::indexAt( int row, int column ) const
{
    const auto item = layoutItemAt( row, column );
    if ( item )
        return q_items.indexOf( item );

    return -1;
}

QskLayoutItem* QskLayoutEngine::layoutItemAt( int index ) const
{
    if ( index < 0 || index >= q_items.count() )
        return nullptr;

    return static_cast< QskLayoutItem* >( q_items[index] );
}

QskLayoutItem* QskLayoutEngine::layoutItemAt( int row, int column ) const
{
    if ( row < 0 || row >= rowCount() || column < 0 || column >= columnCount() )
        return nullptr;

    return static_cast< QskLayoutItem* >( itemAt( row, column ) );
}

QskLayoutItem* QskLayoutEngine::layoutItemAt(
    int row, int column, Qt::Orientation orientation ) const
{
    if ( orientation == Qt::Horizontal )
        return layoutItemAt( row, column );
    else
        return layoutItemAt( column, row );
}

int QskLayoutEngine::indexOf( const QQuickItem* item ) const
{
    // linear search might become slow for many items,
    // better introduce some sort of hash table TODO ...

    for ( int i = q_items.count() - 1; i >= 0; --i )
    {
        const auto layoutItem = static_cast< const QskLayoutItem* >( q_items[i] );
        if ( layoutItem->item() == item )
            return i;
    }

    return -1;
}

QSizeF QskLayoutEngine::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    const LayoutStyleInfo styleInfo;
    return QGridLayoutEngine::sizeHint( which, constraint, &styleInfo );
}

qreal QskLayoutEngine::spacing( Qt::Orientation orientation ) const
{
    const LayoutStyleInfo styleInfo;
    return QGridLayoutEngine::spacing( orientation, &styleInfo );
}

qreal QskLayoutEngine::defaultSpacing( Qt::Orientation orientation )
{
    return LayoutStyleInfo().spacing( orientation );
}

#if 1
// QGridLayout or here ???
QSize QskLayoutEngine::requiredCells() const
{
    int lastRow = -1;
    int lastColumn = -1;

    for ( int i = 0; i < itemCount(); i++ )
    {
        const QskLayoutItem* l = layoutItemAt( i );
        if ( l->isIgnored() )
            continue;

        const int col = l->hasUnlimitedSpan( Qt::Horizontal )
            ? l->firstColumn() + 1 : l->lastColumn();

        if ( col > lastColumn )
            lastColumn = col;

        const int row = l->hasUnlimitedSpan( Qt::Vertical )
            ? l->firstRow() + 1 : l->lastRow();

        if ( row > lastRow )
            lastRow = row;
    }

    return QSize( lastColumn + 1, lastRow + 1 );
}

void QskLayoutEngine::adjustSpans( int numRows, int numColumns )
{
    for ( int i = 0; i < itemCount(); i++ )
    {
        QskLayoutItem* l = layoutItemAt( i );

        if ( l->hasUnlimitedSpan( Qt::Horizontal ) )
            l->setRowSpan( numColumns - l->firstColumn(), Qt::Horizontal );

        if ( l->hasUnlimitedSpan( Qt::Vertical ) )
            l->setRowSpan( numRows - l->firstRow(), Qt::Vertical );
    }
}

#endif

