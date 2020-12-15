/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSimpleListBox.h"
#include "QskAspect.h"
#include "QskFunctions.h"

#include <qfontmetrics.h>

static inline qreal qskMaxWidth(
    const QFont& font, const QStringList& list )
{
    const QFontMetricsF fm( font );

    qreal max = 0.0;
    for ( int i = 0; i < list.size(); i++ )
    {
        const qreal w = qskHorizontalAdvance( fm, list[ i ] );
        if ( w > max )
            max = w;
    }

    return max;
}

class QskSimpleListBox::PrivateData
{
  public:
    PrivateData()
        : maxTextWidth( 0.0 )
        , columnWidthHint( 0.0 )
    {
    }

    // one column at the moment only
    qreal maxTextWidth;
    qreal columnWidthHint;

    QStringList entries;
};

QskSimpleListBox::QskSimpleListBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    connect( this, &Inherited::selectedRowChanged,
        this, [ this ]( int row ) { Q_EMIT selectedEntryChanged( entryAt( row ) ); } );
}

QskSimpleListBox::~QskSimpleListBox()
{
}

QString QskSimpleListBox::entryAt( int row ) const
{
    if ( row >= 0 && row < m_data->entries.size() )
        return m_data->entries[ row ];

    return QString();
}

QVariant QskSimpleListBox::valueAt( int row, int col ) const
{
    if ( col == 0 && row >= 0 && row < m_data->entries.size() )
        return m_data->entries[ row ];

    return QVariant();
}

void QskSimpleListBox::setColumnWidthHint( int column, qreal width )
{
    if ( column != 0 )
        return;

    if ( width != m_data->columnWidthHint )
    {
        m_data->columnWidthHint = qMax( width, qreal( 0.0 ) );

        if ( m_data->columnWidthHint > 0.0 )
            m_data->maxTextWidth = m_data->columnWidthHint;
        else
            m_data->maxTextWidth = qskMaxWidth( effectiveFont( Text ), m_data->entries );

        updateScrollableSize();
    }
}

qreal QskSimpleListBox::columnWidthHint( int column ) const
{
    if ( column == 0 )
        return m_data->columnWidthHint;

    return 0.0;
}

void QskSimpleListBox::insert( const QStringList& list, int index )
{
    if ( list.isEmpty() )
        return;

    if ( m_data->columnWidthHint <= 0.0 )
    {
        const auto w = qskMaxWidth( effectiveFont( Text ), list );
        if ( w > m_data->maxTextWidth )
            m_data->maxTextWidth = w;
    }

    if ( m_data->entries.isEmpty() )
    {
        m_data->entries = list;
    }
    else if ( index < 0 || index >= m_data->entries.size() )
    {
        m_data->entries += list;
    }
    else
    {
        // is there no better way ???
        for ( int i = 0; i < list.size(); i++ )
            m_data->entries.insert( index + i, list[ i ] );
    }

    propagateEntries();
}

void QskSimpleListBox::setEntries( const QStringList& entries )
{
    if ( m_data->entries.isEmpty() && entries.isEmpty() )
        return;

    m_data->entries.clear();

    if ( m_data->columnWidthHint <= 0.0 )
        m_data->maxTextWidth = 0.0;

    insert( entries, -1 );
}

QStringList QskSimpleListBox::entries() const
{
    return m_data->entries;
}

void QskSimpleListBox::insert( const QString& text, int index )
{
    if ( m_data->columnWidthHint <= 0.0 )
    {
        const auto w = qskHorizontalAdvance( effectiveFont( Cell ), text );
        if ( w > m_data->maxTextWidth )
            m_data->maxTextWidth = w;
    }

    if ( index < 0 )
        m_data->entries.append( text );
    else
        m_data->entries.insert( index, text );

    propagateEntries();
}

void QskSimpleListBox::removeAt( int index )
{
    auto& entries = m_data->entries;

    if ( index < 0 || index >= entries.size() )
        return;

    if ( m_data->columnWidthHint <= 0.0 )
    {
        const auto w = qskHorizontalAdvance( effectiveFont( Cell ), entries[ index ] );
        if ( w >= m_data->maxTextWidth )
            m_data->maxTextWidth = qskMaxWidth( effectiveFont( Text ), entries );
    }
    else
    {
        entries.removeAt( index );
    }

    propagateEntries();

    int row = selectedRow();
    if ( row == index )
    {
        row = qMin( row, m_data->entries.size() - 1 );

        if ( row != selectedRow() )
            setSelectedRow( row );
        else
            Q_EMIT selectedRowChanged( row );
    }
}

void QskSimpleListBox::removeBulk( int from, int to )
{
    if ( from < 0 )
        from = 0;

    if ( to < 0 || to >= m_data->entries.size() - 1 )
        to = m_data->entries.size() - 1;

    if ( to < from )
        return;

    for ( int i = to; i >= from; i-- )
        m_data->entries.removeAt( i );

    if ( m_data->columnWidthHint <= 0.0 )
        m_data->maxTextWidth = qskMaxWidth( effectiveFont( Text ), m_data->entries );

    propagateEntries();

    int row = selectedRow();
    if ( row >= 0 )
    {
        if ( m_data->entries.isEmpty() )
        {
            row = -1;
        }
        else if ( row < from )
        {
            // nothing to do
        }
        else if ( row <= to )
        {
            // we might end up here with the same row TODO ...
            row = qMin( from, m_data->entries.size() - 1 );
        }
        else
        {
            row -= ( to - from + 1 );
        }

        if ( row != selectedRow() )
        {
            setSelectedRow( row );
        }
    }
}

void QskSimpleListBox::clear()
{
    if ( m_data->entries.isEmpty() )
        return;

    m_data->entries.clear();

    if ( m_data->columnWidthHint <= 0.0 )
        m_data->maxTextWidth = 0.0;

    propagateEntries();
    setSelectedRow( -1 );
}

void QskSimpleListBox::propagateEntries()
{
#if 1
    // when removing we might have lost the selected row -> what to do then ?
#endif
    updateScrollableSize();
    update();

    Q_EMIT entriesChanged();
}

int QskSimpleListBox::rowCount() const
{
    return m_data->entries.size();
}

int QskSimpleListBox::columnCount() const
{
    return 1;
}

qreal QskSimpleListBox::columnWidth( int col ) const
{
    if ( col >= columnCount() )
        return 0.0;

    const auto padding = paddingHint( Cell );
    return m_data->maxTextWidth + padding.left() + padding.right();
}

qreal QskSimpleListBox::rowHeight() const
{
    const auto padding = paddingHint( Cell );
    const QFontMetricsF fm( effectiveFont( Text ) );

    return fm.height() + padding.top() + padding.bottom();
}

#include "moc_QskSimpleListBox.cpp"
