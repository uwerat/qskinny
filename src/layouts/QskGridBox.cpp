/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGridBox.h"
#include "QskGridLayoutEngine.h"
#include "QskLayoutConstraint.h"
#include "QskEvent.h"
#include <algorithm>

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

static void qskUpdateFocusChain(
    QskGridBox* box, const QskGridLayoutEngine* engine,
    QQuickItem* item, const QRect& grid )
{
    auto comparePosition =
        [item, engine]( const QPoint& pos, const QQuickItem* child )
        {
            if ( item != child )
            {
                const int index = engine->indexOf( child );
                if ( index >= 0 )
                {
                    const auto grid = engine->gridAt( index );
                    if ( pos.y() < grid.y() )
                        return true;

                    if ( pos.y() == grid.y() && pos.x() < grid.x() )
                        return true;
                }
            }

            return false;
        };

    const auto children = box->childItems();

    auto it = std::upper_bound( children.begin(), children.end(),
        grid.topLeft(), comparePosition );

    if ( it != children.end() )
        item->stackBefore( *it );
}

class QskGridBox::PrivateData
{
  public:
    QskGridLayoutEngine engine;
    bool blockAutoRemove = false;
};

QskGridBox::QskGridBox( QQuickItem* parent )
    : QskBox( false, parent )
    , m_data( new PrivateData() )
{
}

QskGridBox::~QskGridBox()
{
}

int QskGridBox::addItem( QQuickItem* item,
    int row, int column, int rowSpan, int columnSpan,
    Qt::Alignment alignment )
{
    if ( item == nullptr || row < 0 || column < 0 )
        return -1;

    rowSpan = qMax( rowSpan, -1 );
    columnSpan = qMax( columnSpan, -1 );

    auto& engine = m_data->engine;

    const QRect itemGrid( column, row, columnSpan, rowSpan );
    int index = -1;

    if ( item->parentItem() == this )
    {
        index = indexOf( item );
        if ( index >= 0 )
        {
            if ( engine.gridAt( index ) == itemGrid )
            {
                if ( engine.setAlignmentAt( index, alignment ) )
                    polish();

                return index;
            }
        }
    }

    if ( index < 0 )
    {
        if ( item->parent() == nullptr )
            item->setParent( this );

        if ( item->parentItem() != this )
            item->setParentItem( this );

        qskSetItemActive( this, item, true );
        index = engine.insertItem( item, itemGrid, alignment );
    }

    if ( engine.count() > 1 )
        qskUpdateFocusChain( this, &engine, item, itemGrid );

    resetImplicitSize();
    polish();

    return index;
}

int QskGridBox::addSpacer( qreal spacing,
    int row, int column, int rowSpan, int columnSpan )
{
    const int index = m_data->engine.insertSpacer(
        spacing, QRect( column, row, columnSpan, rowSpan ) );

    resetImplicitSize();
    polish();

    return index;
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
    m_data->blockAutoRemove = true;

    for ( int i = 0; i < count(); i++ )
    {
        if ( auto item = itemAtIndex( i ) )
        {
            qskSetItemActive( this, item, false );

            if( autoDelete && ( item->parent() == this ) )
                delete item;
            else
                item->setParentItem( nullptr );
        }
    }

    m_data->blockAutoRemove = false;

    m_data->engine.clear();
}

int QskGridBox::count() const
{
    return m_data->engine.count();
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

QRect QskGridBox::gridOfIndex( int index ) const
{
    return m_data->engine.gridAt( index );
}

QRect QskGridBox::effectiveGridOfIndex( int index ) const
{
    return m_data->engine.effectiveGridAt( index );
}

void QskGridBox::setDefaultAlignment( Qt::Alignment alignment )
{
    if ( m_data->engine.setDefaultAlignment( alignment ) )
        Q_EMIT defaultAlignmentChanged();
}

Qt::Alignment QskGridBox::defaultAlignment() const
{
    return m_data->engine.defaultAlignment();
}

void QskGridBox::setSpacing( Qt::Orientations orientations, qreal spacing )
{
    if ( m_data->engine.setSpacing( spacing, orientations ) )
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
            setSpacing( o, m_data->engine.defaultSpacing( o ) );
    }
}

qreal QskGridBox::spacing( Qt::Orientation orientation ) const
{
    return m_data->engine.spacing( orientation );
}

void QskGridBox::setRowStretchFactor( int row, int stretch )
{
    if ( m_data->engine.setStretchFactor( row, stretch, Qt::Vertical ) )
        polish();
}

int QskGridBox::rowStretchFactor( int row ) const
{
    return m_data->engine.stretchFactor( row, Qt::Vertical );
}

void QskGridBox::setColumnStretchFactor( int column, int stretch )
{
    if ( m_data->engine.setStretchFactor( column, stretch, Qt::Horizontal ) )
        polish();
}

int QskGridBox::columnStretchFactor( int column ) const
{
    return m_data->engine.stretchFactor( column, Qt::Horizontal );
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

void QskGridBox::setAlignment( const QQuickItem* item, Qt::Alignment alignment )
{
    auto& engine = m_data->engine;

    const int index = engine.indexOf( item );
    if ( index >= 0 )
    {
        if ( engine.setAlignmentAt( index, alignment ) )
            polish();
    }
}

Qt::Alignment QskGridBox::alignment( const QQuickItem* item ) const
{
    const auto& engine = m_data->engine;
    return engine.alignmentAt( engine.indexOf( item ) );
}

void QskGridBox::setRetainSizeWhenHidden( const QQuickItem* item, bool on )
{
    auto& engine = m_data->engine;

    const int index = engine.indexOf( item );
    if ( index >= 0 )
    {
        if ( engine.setRetainSizeWhenHiddenAt( index, on ) )
            invalidate();
    }
}

bool QskGridBox::retainSizeWhenHidden( const QQuickItem* item ) const
{
    const auto& engine = m_data->engine;
    return engine.retainSizeWhenHiddenAt( engine.indexOf( item ) );
}

void QskGridBox::setRowSizeHint( int row, Qt::SizeHint which, qreal height )
{
    if ( m_data->engine.setRowSizeHint( row, which, height ) )
        polish();
}

qreal QskGridBox::rowSizeHint( int row, Qt::SizeHint which ) const
{
    return m_data->engine.rowSizeHint( row, which );
}

void QskGridBox::setColumnSizeHint( int column, Qt::SizeHint which, qreal width )
{
    if ( m_data->engine.setColumnSizeHint( column, which, width ) )
        polish();
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
    if ( count() == 0 )
        return QSizeF( 0, 0 );

    return m_data->engine.sizeHint( Qt::PreferredSize, QSizeF() );
}

qreal QskGridBox::heightForWidth( qreal width ) const
{
    auto constrainedHeight =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal width )
        {
            const QSizeF constraint( width, -1 );
            return m_data->engine.sizeHint( Qt::PreferredSize, constraint ).height();
        };

    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::HeightForWidth, this, width, constrainedHeight );
}

qreal QskGridBox::widthForHeight( qreal height ) const
{
    auto constrainedWidth =
        [this]( QskLayoutConstraint::Type, const QskControl*, qreal height )
        {
            const QSizeF constraint( -1, height );
            return m_data->engine.sizeHint( Qt::PreferredSize, constraint ).width();
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
            if ( !m_data->blockAutoRemove )
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
