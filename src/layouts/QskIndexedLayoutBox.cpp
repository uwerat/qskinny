/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskIndexedLayoutBox.h"
#include "QskLayoutEngine.h"
#include "QskLayoutItem.h"
#include <limits>

class QskIndexedLayoutBox::PrivateData
{
public:
    PrivateData():
        autoAddChildren( true ),
        blockChildAdded( false ),
        defaultAlignment( Qt::AlignLeft | Qt::AlignVCenter )
    {
    }

    bool autoAddChildren : 1;
    bool blockChildAdded : 1;

    /*
       QGridLayoutEngine is supposed to find the alignment
       ( see: QGridLayoutEngine::effectiveAlignment ) by looking up in:
            item -> row/column -> layout default.

       Unfortunatly the layout default can't be modified without accessing
       private methods and - for some reason worse - QGridLayoutEngine::effectiveAlignment
       does not fall back to the layout default for the horizontal alignment.

       But as we don't offer setting the row/column alignment at the public API
       of QskIndexedLayoutBox we work around by using them instead.
     */
    Qt::Alignment defaultAlignment;
};

QskIndexedLayoutBox::QskIndexedLayoutBox( QQuickItem* parent ):
    QskLayout( parent ),
    m_data( new PrivateData() )
{
    // classBegin/componentComplete -> setActive( false/true ) ?
}

QskIndexedLayoutBox::~QskIndexedLayoutBox()
{
}

void QskIndexedLayoutBox::setAutoAddChildren( bool on )
{
    if ( on != m_data->autoAddChildren )
    {
        m_data->autoAddChildren = on;
        Q_EMIT autoAddChildrenChanged();
    }
}

bool QskIndexedLayoutBox::autoAddChildren() const
{
    return m_data->autoAddChildren;
}

void QskIndexedLayoutBox::setDefaultAlignment( Qt::Alignment alignment )
{
    bool hasChanged = false;

    const Qt::Alignment alignV = alignment & Qt::AlignVertical_Mask;
    if ( alignV != ( m_data->defaultAlignment & Qt::AlignVertical_Mask ) )
    {
        hasChanged = true;

        for ( int row = 0; row < engine().rowCount(); row++ )
            engine().setRowAlignment( row, alignV, Qt::Vertical );
    }

    const Qt::Alignment alignH = alignment & Qt::AlignHorizontal_Mask;
    if ( alignH != ( m_data->defaultAlignment & Qt::AlignHorizontal_Mask ) )
    {
        hasChanged = true;

        for ( int col = 0; col < engine().columnCount(); col++ )
            engine().setRowAlignment( col, alignH, Qt::Horizontal );
    }

    if ( hasChanged )
    {
        m_data->defaultAlignment = alignment;
        Q_EMIT defaultAlignmentChanged();
    }
}

Qt::Alignment QskIndexedLayoutBox::defaultAlignment() const
{
    return m_data->defaultAlignment;
}

void QskIndexedLayoutBox::addItem(
    QQuickItem* item, Qt::Alignment alignment )
{
    insertItem( -1, item, alignment );
}

void QskIndexedLayoutBox::insertItem(
    int index, QQuickItem* item, Qt::Alignment alignment )
{
    if ( item == nullptr )
        return;

    if ( item->parentItem() == this )
    {
        const int oldIndex = indexOf( item );
        if ( oldIndex >= 0  )
        {
            // the item has been inserted before

            const bool doAppend = index < 0 || index >= itemCount();

            if ( ( index == oldIndex ) ||
                ( doAppend && oldIndex == itemCount() - 1 ) )
            {
                // already at its position, nothing to do
                return;
            }

            removeAt( oldIndex );
        }
    }

    QskLayoutItem* layoutItem = new QskLayoutItem( item, 0, 0 );
    layoutItem->setAlignment( alignment );

    insertLayoutItem( layoutItem, index );
}

void QskIndexedLayoutBox::setAlignment( int index, Qt::Alignment alignment )
{
    QskLayoutItem* layoutItem = engine().layoutItemAt( index );
    if ( layoutItem && alignment != layoutItem->alignment() )
    {
        layoutItem->setAlignment( alignment );
        activate(); // invalidate() ???
    }
}

Qt::Alignment QskIndexedLayoutBox::alignment( int index ) const
{
    QskLayoutItem* layoutItem = engine().layoutItemAt( index );
    if ( layoutItem )
        return layoutItem->alignment();

    return Qt::Alignment();
}

void QskIndexedLayoutBox::setAlignment( QQuickItem* item, Qt::Alignment alignment )
{
    setAlignment( engine().indexOf( item ), alignment );
}

Qt::Alignment QskIndexedLayoutBox::alignment( QQuickItem* item ) const
{
    return alignment( engine().indexOf( item ) );
}

void QskIndexedLayoutBox::insertLayoutItem(
    QskLayoutItem* layoutItem, int index )
{
    const int numItems = itemCount();
    if ( index < 0 || index > numItems )
        index = numItems;

    setupLayoutItem( layoutItem, index );

    const int rowCount = engine().rowCount();
    const int columnCount = engine().columnCount();

    // not exception safe !!
    m_data->blockChildAdded = true;
    insertItemInternal( layoutItem, index );
    m_data->blockChildAdded = false;

    if ( rowCount != engine().rowCount() )
    {
        const Qt::Alignment alignV = m_data->defaultAlignment & Qt::AlignVertical_Mask;
        for ( int row = 0; row < engine().rowCount(); row++ )
            engine().setRowAlignment( row, alignV, Qt::Vertical );
    }

    if ( columnCount != engine().columnCount() )
    {
        const Qt::Alignment alignH = m_data->defaultAlignment & Qt::AlignHorizontal_Mask;
        for ( int col = 0; col < engine().columnCount(); col++ )
            engine().setRowAlignment( col, alignH, Qt::Horizontal );
    }

    layoutItemInserted( layoutItem, index );
}

void QskIndexedLayoutBox::itemChange(
    QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value )
{
    switch( change )
    {
        case QQuickItem::ItemChildAddedChange:
        {
            if ( m_data->autoAddChildren && !m_data->blockChildAdded )
            {
                if ( !qskIsTransparentForPositioner( value.item ) )
                    addItem( value.item );
            }

            break;
        }
        case QQuickItem::ItemChildRemovedChange:
        {
            removeItem( value.item );
            break;
        }
        default:
        {
            break;
        }
    }

    return Inherited::itemChange( change, value );
}

#include "moc_QskIndexedLayoutBox.cpp"
