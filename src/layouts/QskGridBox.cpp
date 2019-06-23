/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGridBox.h"
#include "QskGridLayoutEngine.h"
#include "QskLayoutConstraint.h"
#include "QskEvent.h"

static void qskSetItemActive( QObject* receiver, const QQuickItem* item, bool on )
{
    if ( ( item == nullptr ) || ( qskControlCast( item ) != nullptr ) )
        return;

    /*
        For QQuickItems not being derived from QskControl we manually
        send QEvent::LayoutRequest events.
     */

    if ( on )
    {
        auto sendLayoutRequest =
            [receiver]()
            {
                QEvent event( QEvent::LayoutRequest );
                QCoreApplication::sendEvent( receiver, &event );
            };

        QObject::connect( item, &QQuickItem::implicitWidthChanged,
            receiver, sendLayoutRequest );

        QObject::connect( item, &QQuickItem::implicitHeightChanged,
            receiver, sendLayoutRequest );

        QObject::connect( item, &QQuickItem::visibleChanged,
            receiver, sendLayoutRequest );
    }
    else
    {
        QObject::disconnect( item, &QQuickItem::implicitWidthChanged, receiver, nullptr );
        QObject::disconnect( item, &QQuickItem::implicitHeightChanged, receiver, nullptr );
        QObject::disconnect( item, &QQuickItem::visibleChanged, receiver, nullptr );
    }
}

class QskGridBox::PrivateData
{
  public:
    QskGridLayoutEngine engine;
};

QskGridBox::QskGridBox( QQuickItem* parent )
    : QskBox( false, parent )
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
    if ( item == nullptr )
        return;

    if ( item->parent() == nullptr )
        item->setParent( this );

    if ( item->parentItem() != this )
        item->setParentItem( this );

    qskSetItemActive( this, item, true );

    // What about the focus tab chain - TODO ... ????
    // check if item is already inserted ???

    m_data->engine.insertItem(
        item, row, column, rowSpan, columnSpan, alignment );

    resetImplicitSize();
    polish();

}

void QskGridBox::removeAt( int index )
{
    auto& engine = m_data->engine;

    if ( auto item = engine.itemAt( index ) )
        qskSetItemActive( this, item, false );

    engine.removeAt( index );

    resetImplicitSize();
    polish();
}

void QskGridBox::removeItem( const QQuickItem* item )
{
    removeAt( indexOf( item ) );
}

void QskGridBox::clear( bool autoDelete )
{
    for ( int i = itemCount() - 1; i >= 0; i-- )
    {
        auto item = itemAtIndex( i );

        removeAt( i );

        if( item )
        {
            if( autoDelete && ( item->parent() == this ) )
                delete item;
            else
                item->setParentItem( nullptr );
        }
    }
}

int QskGridBox::itemCount() const
{
    return m_data->engine.itemCount();
}

int QskGridBox::rowCount() const
{
    return m_data->engine.rowCount();
}

int QskGridBox::columnCount() const
{
    return m_data->engine.columnCount();
}

QQuickItem* QskGridBox::itemAtIndex( int index ) const
{
    return m_data->engine.itemAt( index );
}

int QskGridBox::indexOf( const QQuickItem* item ) const
{
    return m_data->engine.indexOf( item );
}

QQuickItem* QskGridBox::itemAt( int row, int column ) const
{
    return m_data->engine.itemAt( row, column );
}

int QskGridBox::indexAt( int row, int column ) const
{
    return m_data->engine.indexAt( row, column );
}

int QskGridBox::rowOfIndex( int index ) const
{
    return m_data->engine.rowOfIndex( index );
}

int QskGridBox::rowSpanOfIndex( int index ) const
{
    return m_data->engine.rowSpanOfIndex( index );
}

int QskGridBox::columnOfIndex( int index ) const
{
    return m_data->engine.columnOfIndex( index );
}

int QskGridBox::columnSpanOfIndex( int index ) const
{
    return m_data->engine.columnSpanOfIndex( index );
}

void QskGridBox::setSpacing( Qt::Orientations orientations, qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    bool doUpdate = false;

    auto& engine = m_data->engine;

    for ( const auto o : { Qt::Horizontal, Qt::Vertical } )
    {
        if ( orientations & o )
        {
            if ( spacing != engine.spacing( o ) )
            {
                engine.setSpacing( o, spacing );
                doUpdate = true;
            }
        }
    }

    if ( doUpdate )
    {
        resetImplicitSize();
        polish();
    }
}

void QskGridBox::resetSpacing( Qt::Orientations orientations )
{
    for ( const auto o : { Qt::Horizontal, Qt::Vertical } )
    {
        if ( orientations & o )
            setSpacing( o, QskGridLayoutEngine::defaultSpacing( o ) );
    }
}

qreal QskGridBox::spacing( Qt::Orientation orientation ) const
{
    return m_data->engine.spacing( orientation );
}

void QskGridBox::setRowSpacing( int row, qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    auto& engine = m_data->engine;

    if ( spacing != engine.spacingAt( Qt::Vertical, row ) )
    {
        engine.setSpacingAt( Qt::Vertical, row, spacing );
        polish();
    }
}

qreal QskGridBox::rowSpacing( int row ) const
{
    return m_data->engine.spacingAt( Qt::Vertical, row );
}

void QskGridBox::setColumnSpacing( int column, qreal spacing )
{
    spacing = qMax( spacing, 0.0 );

    auto& engine = m_data->engine;

    if ( spacing != engine.spacingAt( Qt::Horizontal, column ) )
    {
        engine.setSpacingAt( Qt::Horizontal, column, spacing );
        polish();
    }
}

qreal QskGridBox::columnSpacing( int column ) const
{
    return m_data->engine.spacingAt( Qt::Horizontal, column );
}

void QskGridBox::setRowStretchFactor( int row, int stretch )
{
    auto& engine = m_data->engine;

    if ( stretch != engine.stretchFactorAt( Qt::Vertical, row ) )
    {
        engine.setStretchFactorAt( Qt::Vertical, row, stretch );
        polish();
    }
}

int QskGridBox::rowStretchFactor( int row ) const
{
    return m_data->engine.stretchFactorAt( Qt::Vertical, row );
}

void QskGridBox::setColumnStretchFactor( int column, int stretch )
{
    auto& engine = m_data->engine;

    if ( stretch != engine.stretchFactorAt( Qt::Horizontal, column ) )
    {
        engine.setStretchFactorAt( Qt::Horizontal, column, stretch );
        polish();
    }
}

int QskGridBox::columnStretchFactor( int column ) const
{
    return m_data->engine.stretchFactorAt( Qt::Horizontal, column );
}

void QskGridBox::setRowFixedHeight( int row, qreal height )
{
    setRowSizeHint( row, Qt::MinimumSize, height );
    setRowSizeHint( row, Qt::MaximumSize, height );
}

void QskGridBox::setColumnFixedWidth( int column, qreal width )
{
    setColumnSizeHint( column, Qt::MinimumSize, width );
    setColumnSizeHint( column, Qt::MaximumSize, width );
}

void QskGridBox::setRowAlignment( int row, Qt::Alignment alignment )
{
    auto& engine = m_data->engine;

    if ( engine.alignmentAt( Qt::Vertical, row ) != alignment )
    {
        engine.setAlignmentAt( Qt::Vertical, row, alignment );
        polish();
    }
}

Qt::Alignment QskGridBox::rowAlignment( int row ) const
{
    return m_data->engine.alignmentAt( Qt::Vertical, row );
}

void QskGridBox::setColumnAlignment( int column, Qt::Alignment alignment )
{
    auto& engine = m_data->engine;

    if ( engine.alignmentAt( Qt::Horizontal, column ) != alignment )
    {
        engine.setAlignmentAt( Qt::Horizontal, column, alignment );
        polish();
    }
}

Qt::Alignment QskGridBox::columnAlignment( int column ) const
{
    return m_data->engine.alignmentAt( Qt::Horizontal, column );
}

void QskGridBox::setAlignment( const QQuickItem* item, Qt::Alignment alignment )
{
    auto& engine = m_data->engine;

    if ( engine.alignmentOf( item ) != alignment )
    {
        engine.setAlignmentOf( item, alignment );
        polish();
    }
}

Qt::Alignment QskGridBox::alignment( const QQuickItem* item ) const
{
    return m_data->engine.alignmentOf( item );
}

void QskGridBox::setRetainSizeWhenHidden( const QQuickItem* item, bool on )
{
    auto& engine = m_data->engine;

    if ( engine.retainSizeWhenHiddenOf( item ) != on )
    {
        engine.setRetainSizeWhenHiddenOf( item, on );
        invalidate();
    }
}

bool QskGridBox::retainSizeWhenHidden( const QQuickItem* item ) const
{
    return m_data->engine.retainSizeWhenHiddenOf( item );
}

void QskGridBox::setRowSizeHint( int row, Qt::SizeHint which, qreal height )
{
    auto& engine = m_data->engine;

    if ( height != engine.rowSizeHint( row, which ) )
    {
        engine.setRowSizeHint( row, which, height );
        polish();
    }
}

qreal QskGridBox::rowSizeHint( int row, Qt::SizeHint which ) const
{
    return m_data->engine.rowSizeHint( row, which );
}

void QskGridBox::setColumnSizeHint( int column, Qt::SizeHint which, qreal width )
{
    auto& engine = m_data->engine;

    if ( width != engine.columnSizeHint( column, which ) )
    {
        engine.setColumnSizeHint( column, which, width );
        polish();
    }
}

qreal QskGridBox::columnSizeHint( int column, Qt::SizeHint which ) const
{
    return m_data->engine.columnSizeHint( column, which );
}

void QskGridBox::invalidate()
{
    m_data->engine.invalidate();

    resetImplicitSize();
    polish();
}

void QskGridBox::updateLayout()
{
    if ( !maybeUnresized() )
        m_data->engine.setGeometries( layoutRect() );
}

QSizeF QskGridBox::contentsSizeHint() const
{
    if ( itemCount() == 0 )
        return QSizeF( 0, 0 );

    return m_data->engine.sizeHint( Qt::PreferredSize, QSizeF() );
}

qreal QskGridBox::heightForWidth( qreal width ) const
{
    auto constrainedHeight =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal width )
        {
            return m_data->engine.heightForWidth( width );
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::HeightForWidth, this, width, constrainedHeight );
}

qreal QskGridBox::widthForHeight( qreal height ) const
{
    auto constrainedWidth =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal height )
        {
            return m_data->engine.widthForHeight( height );
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::WidthForHeight, this, height, constrainedWidth );
}

void QskGridBox::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    Inherited::geometryChangeEvent( event );

    if ( event->isResized() )
        polish();
}

void QskGridBox::itemChange( ItemChange change, const ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch ( change )
    {
        case ItemChildRemovedChange:
        {
            removeItem( value.item );
            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            if ( value.boolValue )
                polish();
            break;
        }
        case QQuickItem::ItemSceneChange:
        {
            if ( value.window )
                polish();
            break;
        }
        default:
            break;
    }
}

bool QskGridBox::event( QEvent* event )
{
    switch ( event->type() )
    {
        case QEvent::LayoutRequest:
        {
            invalidate();
            break;
        }
        case QEvent::LayoutDirectionChange:
        {
            m_data->engine.setVisualDirection(
                layoutMirroring() ? Qt::RightToLeft : Qt::LeftToRight );

            polish();
            break;
        }
        case QEvent::ContentsRectChange:
        {
            polish();
            break;
        }
        default:
            break;
    }

    return Inherited::event( event );
}

#include "moc_QskGridBox.cpp"
