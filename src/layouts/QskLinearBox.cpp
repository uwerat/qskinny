/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLinearBox.h"
#include "QskLayoutEngine.h"
#include "QskLayoutItem.h"

#include <qendian.h>

class QskLinearBox::PrivateData
{
  public:
    PrivateData( Qt::Orientation orient, uint dim )
        : dimension( dim )
        , orientation( orient )
        , transposeAlignments( false )
    {
    }

    uint dimension;
    Qt::Edges extraSpacingAt;

    Qt::Orientation orientation : 2;
    bool transposeAlignments : 1;
};

QskLinearBox::QskLinearBox( QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, std::numeric_limits< uint >::max(), parent )
{
}

QskLinearBox::QskLinearBox( Qt::Orientation orientation, QQuickItem* parent )
    : QskLinearBox( orientation, std::numeric_limits< uint >::max(), parent )
{
}

QskLinearBox::QskLinearBox(
        Qt::Orientation orientation, uint dimension, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( orientation, dimension ) )
{
}

QskLinearBox::~QskLinearBox()
{
}

void QskLinearBox::setDimension( uint dimension )
{
    if ( dimension < 1 )
        dimension = 1;

    if ( dimension != m_data->dimension )
    {
        m_data->dimension = dimension;

        rearrange();
        polish();
    }
}

uint QskLinearBox::dimension() const
{
    return m_data->dimension;
}

void QskLinearBox::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->orientation != orientation )
        transpose();
}

Qt::Orientation QskLinearBox::orientation() const
{
    return m_data->orientation;
}

void QskLinearBox::transpose()
{
    const Qt::Orientation orientation =
        ( m_data->orientation == Qt::Horizontal ) ? Qt::Vertical : Qt::Horizontal;

    const int numItems = itemCount();

    if ( numItems > 0 )
    {
        for ( int i = 0; i < numItems; i++ )
        {
            QskLayoutItem* layoutItem = engine().layoutItemAt( i );

            const int row = layoutItem->firstRow( Qt::Horizontal );
            const int col = layoutItem->firstRow( Qt::Vertical );

            engine().removeItem( layoutItem );

            layoutItem->setFirstRow( row, Qt::Vertical );
            layoutItem->setFirstRow( col, Qt::Horizontal );

#if 1
            if ( m_data->transposeAlignments )
            {
                // Is it worth to blow the API with this flag, or would
                // it be even better to have an indvidual flag for each
                // item - and what about the size policies: do we want to
                // transpose them too ?

                const auto alignment = static_cast< Qt::Alignment >(
                    qbswap( static_cast< quint16 >( layoutItem->alignment() ) ) );

                layoutItem->setAlignment( alignment );
            }
#endif

            if ( layoutItem->item() == nullptr )
            {
                // a spacing or stretch
                layoutItem->setSpacingHint(
                    layoutItem->spacingHint().transposed() );
            }

            engine().insertLayoutItem( layoutItem, i );
        }

        invalidate();
    }

    m_data->orientation = orientation;
    Q_EMIT orientationChanged();
}

void QskLinearBox::setSpacing( qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    if ( spacing != engine().spacing( Qt::Horizontal ) )
    {
        engine().setSpacing( spacing, Qt::Horizontal | Qt::Vertical );
        activate();

        Q_EMIT spacingChanged();
    }
}

void QskLinearBox::resetSpacing()
{
    const qreal spacing = QskLayoutEngine::defaultSpacing( Qt::Horizontal );
    setSpacing( spacing );
}

qreal QskLinearBox::spacing() const
{
    // do we always want to have the same spacing for both orientations
    return engine().spacing( Qt::Horizontal );
}

void QskLinearBox::setExtraSpacingAt( Qt::Edges edges )
{
    if ( edges != m_data->extraSpacingAt )
    {
        m_data->extraSpacingAt = edges;
        activate();

        Q_EMIT extraSpacingAtChanged();
    }
}

Qt::Edges QskLinearBox::extraSpacingAt() const
{
    return m_data->extraSpacingAt;
}

void QskLinearBox::addSpacer( qreal spacing, int stretchFactor )
{
    insertSpacer( -1, spacing, stretchFactor );
}

void QskLinearBox::insertSpacer( int index, qreal spacing, int stretchFactor )
{
    spacing = qMax( spacing, 0.0 );
    stretchFactor = qMax( stretchFactor, 0 );

    QskLayoutItem* layoutItem;
    if ( m_data->orientation == Qt::Horizontal )
        layoutItem = new QskLayoutItem( QSizeF( spacing, -1.0 ), stretchFactor, 0, 0 );
    else
        layoutItem = new QskLayoutItem( QSizeF( -1.0, spacing ), stretchFactor, 0, 0 );

#if 1
    if ( stretchFactor >= 0 )
        layoutItem->setStretchFactor( stretchFactor, m_data->orientation ); // already above ???
#endif

    insertLayoutItem( layoutItem, index );
}

void QskLinearBox::addStretch( int stretchFactor )
{
    insertSpacer( -1, 0, stretchFactor );
}

void QskLinearBox::insertStretch( int index, int stretchFactor )
{
    insertSpacer( index, 0, stretchFactor );
}

void QskLinearBox::setStretchFactor( int index, int stretchFactor )
{
    if ( QskLayoutItem* layoutItem = engine().layoutItemAt( index ) )
    {
        if ( layoutItem->stretchFactor( m_data->orientation ) != stretchFactor )
        {
            layoutItem->setStretchFactor( stretchFactor, m_data->orientation );
            // activate();
        }
    }
}

int QskLinearBox::stretchFactor( int index ) const
{
    if ( QskLayoutItem* layoutItem = engine().layoutItemAt( index ) )
        return layoutItem->stretchFactor( m_data->orientation );

    return 0;
}

void QskLinearBox::setStretchFactor( QQuickItem* item, int stretch )
{
    setStretchFactor( engine().indexOf( item ), stretch );
}

int QskLinearBox::stretchFactor( QQuickItem* item ) const
{
    return stretchFactor( engine().indexOf( item ) );
}

void QskLinearBox::setRetainSizeWhenHidden( int index, bool on )
{
    QskLayoutItem* layoutItem = engine().layoutItemAt( index );
    if ( layoutItem && on != layoutItem->retainSizeWhenHidden() )
    {
        layoutItem->setRetainSizeWhenHidden( on );
        invalidate();
    }
}

bool QskLinearBox::retainSizeWhenHidden( int index ) const
{
    QskLayoutItem* layoutItem = engine().layoutItemAt( index );
    if ( layoutItem )
        return layoutItem->retainSizeWhenHidden();

    return false;
}

void QskLinearBox::setRetainSizeWhenHidden( QQuickItem* item, bool on )
{
    setRetainSizeWhenHidden( engine().indexOf( item ), on );
}

bool QskLinearBox::retainSizeWhenHidden( QQuickItem* item ) const
{
    return retainSizeWhenHidden( engine().indexOf( item ) );
}

void QskLinearBox::setRowSpacing( int row, qreal spacing )
{
    if ( row >= 0 )
    {
        engine().setRowSpacing( row, spacing, Qt::Horizontal );
        activate();
    }
}

qreal QskLinearBox::rowSpacing( int row ) const
{
    return engine().rowSpacing( row, Qt::Horizontal );
}

void QskLinearBox::setColumnSpacing( int column, qreal spacing )
{
    if ( column >= 0 )
    {
        engine().setRowSpacing( column, spacing, Qt::Vertical );
        activate();
    }
}

qreal QskLinearBox::columnSpacing( int column ) const
{
    return engine().rowSpacing( column, Qt::Vertical );
}

void QskLinearBox::setRowStretchFactor( int row, int stretchFactor )
{
    if ( row >= 0 )
    {
        engine().setRowStretchFactor( row, stretchFactor, Qt::Vertical );
        activate();
    }
}

int QskLinearBox::rowStretchFactor( int row ) const
{
    return engine().rowStretchFactor( row, Qt::Vertical );
}

void QskLinearBox::setColumnStretchFactor( int column, int stretchFactor )
{
    if ( column >= 0 )
    {
        engine().setRowStretchFactor( column, stretchFactor, Qt::Horizontal );
        activate();
    }
}

int QskLinearBox::columnStretchFactor( int column ) const
{
    return engine().rowStretchFactor( column, Qt::Horizontal );
}

QSizeF QskLinearBox::contentsSizeHint() const
{
    if ( !isActive() )
        return QSizeF( -1, -1 );

    if ( itemCount() == 0 )
        return QSizeF( 0, 0 );

    return engine().sizeHint( Qt::PreferredSize );
}

qreal QskLinearBox::heightForWidth( qreal width ) const
{
    const auto m = margins();
    width -= m.left() + m.right();

    qreal height = engine().heightForWidth( width );

    height += m.top() + m.bottom();
    return height;
}

qreal QskLinearBox::widthForHeight( qreal height ) const
{
    const auto m = margins();
    height -= m.top() + m.bottom();

    qreal width = engine().widthForHeight( height );

    width += m.left() + m.right();
    return width;
}

void QskLinearBox::setupLayoutItem( QskLayoutItem* layoutItem, int index )
{
    int col = index % m_data->dimension;
    int row = index / m_data->dimension;

    if ( m_data->orientation == Qt::Vertical )
        qSwap( col, row );

    layoutItem->setFirstRow( col, Qt::Horizontal );
    layoutItem->setFirstRow( row, Qt::Vertical );
}

void QskLinearBox::layoutItemInserted( QskLayoutItem*, int index )
{
    if ( index < itemCount() - 1 )
        rearrange();
}

void QskLinearBox::layoutItemRemoved( QskLayoutItem*, int index )
{
    Q_UNUSED( index )
    rearrange();
}

void QskLinearBox::rearrange()
{
    bool doInvalidate = false;

    const int numItems = itemCount();

    for ( int i = 0; i < numItems; i++ )
    {
        int row = i / m_data->dimension;
        int col = i % m_data->dimension;

        if ( m_data->orientation == Qt::Vertical )
            qSwap( col, row );

        QskLayoutItem* layoutItem = engine().layoutItemAt( i );

        if ( layoutItem->firstRow( Qt::Horizontal ) != col ||
            layoutItem->firstRow( Qt::Vertical ) != row )
        {
            engine().removeItem( layoutItem );

            layoutItem->setFirstRow( col, Qt::Horizontal );
            layoutItem->setFirstRow( row, Qt::Vertical );

            engine().insertLayoutItem( layoutItem, i );

            doInvalidate = true;
        }
    }

    if ( doInvalidate )
        invalidate();
}

QRectF QskLinearBox::alignedLayoutRect( const QRectF& rect ) const
{
    if ( m_data->extraSpacingAt == 0 )
        return rect;

    const QskLayoutEngine& engine = this->engine();

    QRectF r = rect;

    // not 100% sure if this works for dynamic constraints
    // and having extraSpacingAt for both directions ...

    if ( ( m_data->extraSpacingAt & Qt::LeftEdge ) ||
        ( m_data->extraSpacingAt & Qt::RightEdge ) )
    {
        bool isExpandable = false;

        for ( int i = 0; i < engine.itemCount(); i++ )
        {
            const QskLayoutItem* item = engine.layoutItemAt( i );

            if ( !item->isIgnored() &&
                ( item->sizePolicy( Qt::Horizontal ) & QskSizePolicy::GrowFlag ) )
            {
                isExpandable = true;
                break;
            }
        }

        if ( !isExpandable )
        {
            const qreal w = engine.widthForHeight( r.height() );

            if ( m_data->extraSpacingAt & Qt::LeftEdge )
            {
                if ( m_data->extraSpacingAt & Qt::RightEdge )
                {
                    r.moveLeft( r.center().x() - w / 2 );
                    r.setWidth( w );
                }
                else
                {
                    r.setLeft( r.right() - w );
                }
            }
            else
            {
                r.setRight( r.left() + w );
            }
        }
    }

    if ( ( m_data->extraSpacingAt & Qt::TopEdge ) ||
        ( m_data->extraSpacingAt & Qt::BottomEdge ) )
    {
        bool isExpandable = false;

        for ( int i = 0; i < engine.itemCount(); i++ )
        {
            const QskLayoutItem* item = engine.layoutItemAt( i );

            if ( !item->isIgnored() &&
                ( item->sizePolicy( Qt::Vertical ) & QskSizePolicy::GrowFlag ) )
            {
                isExpandable = true;
                break;
            }
        }

        if ( !isExpandable )
        {
            const qreal h = engine.heightForWidth( r.width() );

            if ( m_data->extraSpacingAt & Qt::TopEdge )
            {
                if ( m_data->extraSpacingAt & Qt::BottomEdge )
                {
                    r.moveTop( r.center().y() - h / 2 );
                    r.setHeight( h );
                }
                else
                {
                    r.setTop( r.bottom() - h );
                }
            }
            else
            {
                r.setBottom( r.top() + h );
            }
        }
    }

    return r;
}

#include "moc_QskLinearBox.cpp"
