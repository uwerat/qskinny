/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStackBox.h"
#include "QskStackBoxAnimator.h"
#include "QskLayoutConstraint.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <QPointer>

static qreal qskConstrainedValue( QskLayoutConstraint::Type type,
    const QskControl* control, qreal widthOrHeight )
{
    using namespace QskLayoutConstraint;

    auto constrainFunction =
        ( type == WidthForHeight ) ? widthForHeight : heightForWidth;

    qreal constrainedValue = -1;
    auto stackBox = static_cast< const QskStackBox* >( control );

    for ( int i = 0; i < stackBox->itemCount(); i++ )
    {
        if ( const auto item = stackBox->itemAtIndex( i ) )
        {
            const qreal v = constrainFunction( item, widthOrHeight );
            if ( v > constrainedValue )
                constrainedValue = v;
        }
    }

    return constrainedValue;
}

namespace
{
    class ItemInfo
    {
      public:
        inline ItemInfo()
            : item( nullptr )
        {
        }

        inline ItemInfo( Qt::Alignment alignment, QQuickItem* item )
            : alignment( alignment )
            , item( item )
        {
        }

        Qt::Alignment alignment;
        QQuickItem* item;
    };
}

class QskStackBox::PrivateData
{
  public:
    QVector< ItemInfo > itemInfos;
    QPointer< QskStackBoxAnimator > animator;

    int currentIndex = -1;
    Qt::Alignment defaultAlignment = Qt::AlignLeft | Qt::AlignVCenter;
};

QskStackBox::QskStackBox( QQuickItem* parent )
    : QskStackBox( false, parent )
{
}

QskStackBox::QskStackBox( bool autoAddChildren, QQuickItem* parent )
    : QskIndexedLayoutBox( parent )
    , m_data( new PrivateData() )
{
    setAutoAddChildren( autoAddChildren );
}

QskStackBox::~QskStackBox()
{
}

void QskStackBox::setDefaultAlignment( Qt::Alignment alignment )
{
    if ( alignment != m_data->defaultAlignment )
    {
        m_data->defaultAlignment = alignment;
        Q_EMIT defaultAlignmentChanged( alignment );

        polish();
    }
}

Qt::Alignment QskStackBox::defaultAlignment() const
{
    return m_data->defaultAlignment;
}

void QskStackBox::setAnimator( QskStackBoxAnimator* animator )
{
    if ( m_data->animator == animator )
        return;

    if ( m_data->animator )
    {
        m_data->animator->stop();
        delete m_data->animator;
    }

    if ( animator )
    {
        animator->stop();
        animator->setParent( this );
    }

    m_data->animator = animator;
}

const QskStackBoxAnimator* QskStackBox::animator() const
{
    return m_data->animator;
}

QskStackBoxAnimator* QskStackBox::animator()
{
    return m_data->animator;
}

QskStackBoxAnimator* QskStackBox::effectiveAnimator()
{
    if ( m_data->animator )
        return m_data->animator;

    // getting an animation from the skin. TODO ...

    return nullptr;
}

int QskStackBox::itemCount() const
{
    return m_data->itemInfos.count();
}

QQuickItem* QskStackBox::itemAtIndex( int index ) const
{
    if ( index >= 0 && index < m_data->itemInfos.count() )
        return m_data->itemInfos[ index ].item;

    return nullptr;
}

int QskStackBox::indexOf( const QQuickItem* item ) const
{
    if ( item && ( item->parentItem() != this ) )
    {
        for ( int i = 0; i < m_data->itemInfos.count(); i++ )
        {
            if ( item == m_data->itemInfos[i].item )
                return i;
        }
    }

    return -1;
}

QQuickItem* QskStackBox::currentItem() const
{
    return itemAtIndex( m_data->currentIndex );
}

int QskStackBox::currentIndex() const
{
    return m_data->currentIndex;
}

void QskStackBox::setCurrentIndex( int index )
{
    if ( index < 0 || index >= itemCount() )
    {
        // hide the current item
        index = -1;
    }

    if ( index == m_data->currentIndex )
        return;

    // stop and complete the running transition
    auto animator = effectiveAnimator();
    if ( animator )
        animator->stop();

    if ( window() && isVisible() && isInitiallyPainted() && animator )
    {
        // start the animation
        animator->setStartIndex( m_data->currentIndex );
        animator->setEndIndex( index );
        animator->setWindow( window() );
        animator->start();
    }
    else
    {
        auto item1 = itemAtIndex( m_data->currentIndex );
        auto item2 = itemAtIndex( index );

        if ( item1 )
            item1->setVisible( false );

        if ( item2 )
            item2->setVisible( true );
    }

    m_data->currentIndex = index;
    Q_EMIT currentIndexChanged( m_data->currentIndex );
}

void QskStackBox::setCurrentItem( const QQuickItem* item )
{
    setCurrentIndex( indexOf( item ) );
}

void QskStackBox::addItem( QQuickItem* item, Qt::Alignment alignment )
{
    insertItem( -1, item, alignment );
}

void QskStackBox::insertItem(
    int index, QQuickItem* item, Qt::Alignment alignment )
{
    if ( item == nullptr )
        return;

    reparentItem( item );

    if ( qskIsTransparentForPositioner( item ) )
    {
        // giving a warning, or ignoring the insert ???
        qskSetTransparentForPositioner( item, false );
    }

    const bool doAppend = ( index < 0 ) || ( index >= itemCount() );

    if ( item->parentItem() == this )
    {
        const int oldIndex = indexOf( item );
        if ( oldIndex >= 0 )
        {
            // the item had been inserted before

            if ( ( index == oldIndex ) || ( doAppend && ( oldIndex == itemCount() - 1 ) ) )
            {
                // already in place

                auto& itemInfo = m_data->itemInfos[oldIndex];

                if ( alignment != itemInfo.alignment )
                {
                    itemInfo.alignment = alignment;
                    polish();
                }

                return;
            }

            m_data->itemInfos.removeAt( oldIndex );
        }
    }

    if ( doAppend )
        index = itemCount();

    m_data->itemInfos.insert( index, { alignment, item } );

    const int oldCurrentIndex = m_data->currentIndex;

    if ( m_data->itemInfos.count() == 1 )
    {
        m_data->currentIndex = 0;
        item->setVisible( true );
    }
    else
    {
        item->setVisible( false );

        if ( index <= m_data->currentIndex )
            m_data->currentIndex++;
    }

    if ( oldCurrentIndex != m_data->currentIndex )
        Q_EMIT currentIndexChanged( m_data->currentIndex );

    resetImplicitSize();
    polish();
}

void QskStackBox::removeAt( int index )
{
    removeItemInternal( index, false );
}

void QskStackBox::removeItemInternal( int index, bool unparent )
{
    if ( index < 0 || index >= m_data->itemInfos.count() )
        return;

    if ( !unparent )
    {
        if ( auto item = m_data->itemInfos[ index ].item )
        {
            if ( item->parentItem() == this )
                item->setParentItem( nullptr );
        }
    }

    m_data->itemInfos.removeAt( index );

    if ( index <= m_data->currentIndex )
        Q_EMIT currentIndexChanged( --m_data->currentIndex );

    resetImplicitSize();
    polish();
}

void QskStackBox::removeItem( const QQuickItem* item )
{
    removeAt( indexOf( item ) );
}

void QskStackBox::autoAddItem( QQuickItem* item )
{
    removeAt( indexOf( item ) );
}

void QskStackBox::autoRemoveItem( QQuickItem* item )
{
    removeItemInternal( indexOf( item ), false );
}

void QskStackBox::clear( bool autoDelete )
{
    for ( const auto& itemInfo : qskAsConst( m_data->itemInfos ) )
    {
        auto item = itemInfo.item;

        if( autoDelete && ( item->parent() == this ) )
        {
            delete item;
        }
        else
        {
            item->setParentItem( nullptr );
        }
    }

    m_data->itemInfos.clear();

    if ( m_data->currentIndex >= 0 )
    {
        m_data->currentIndex = -1;
        Q_EMIT currentIndexChanged( m_data->currentIndex );
    }
}

void QskStackBox::setAlignment( const QQuickItem* item, Qt::Alignment alignment )
{
    setAlignmentAt( indexOf( item ), alignment );
}

Qt::Alignment QskStackBox::alignment( const QQuickItem* item ) const
{
    return alignmentAt( indexOf( item ) );
}

void QskStackBox::setAlignmentAt( int index, Qt::Alignment alignment )
{
    if ( index < 0 || index >= m_data->itemInfos.count() )
        return;

    m_data->itemInfos[ index ].alignment = alignment;

    if ( index == m_data->currentIndex )
        polish();
}

Qt::Alignment QskStackBox::alignmentAt( int index ) const
{
    if ( index >= 0 && index < m_data->itemInfos.count() )
        return m_data->itemInfos[ index ].alignment;

    return Qt::Alignment();
}

QRectF QskStackBox::geometryForItemAt( int index ) const
{
    const auto r = layoutRect();

    if ( index >= 0 && index < m_data->itemInfos.count() )
    {
        const auto& info = m_data->itemInfos[ index ];

        const auto align = info.alignment ? info.alignment : m_data->defaultAlignment;
        return QskLayoutConstraint::itemRect( info.item, r, align );
    }

    return QRectF( r.x(), r.y(), 0.0, 0.0 );
}

void QskStackBox::updateLayout()
{
    if ( maybeUnresized() )
        return;

    const auto idx = m_data->currentIndex;

    if ( idx >= 0 )
    {
        const auto rect = geometryForItemAt( idx );
        qskSetItemGeometry( m_data->itemInfos[ idx ].item, rect );
    }
}

QSizeF QskStackBox::contentsSizeHint() const
{
#if 1
    if ( itemCount() == 0 )
        return QSizeF( 0, 0 );
#endif

    qreal width = -1;
    qreal height = -1;

    using namespace QskLayoutConstraint;

    int constraintTypes = Unconstrained;

    for ( const auto& itemInfo : qskAsConst( m_data->itemInfos ) )
    {
        const auto item = itemInfo.item;

        const auto type = constraintType( item );
        if ( type != Unconstrained )
        {
            constraintTypes |= type;
        }
        else
        {
            const QSizeF hint = effectiveConstraint( item, Qt::PreferredSize );

            if ( hint.width() >= width )
                width = hint.width();

            if ( hint.height() >= height )
                height = hint.height();
        }
    }

#if 1
    // does this work ???

    if ( constraintTypes & WidthForHeight )
    {
        const QSizeF constraint( -1, height );

        for ( const auto& itemInfo : qskAsConst( m_data->itemInfos ) )
        {
            const auto item = itemInfo.item;

            if ( constraintType( item ) == WidthForHeight )
            {
                const QSizeF hint = QskLayoutConstraint::sizeHint(
                    item, Qt::PreferredSize, constraint );

                width = qMax( width, hint.width() );
            }
        }
    }

    if ( constraintTypes & HeightForWidth )
    {
        const QSizeF constraint( width, -1 );

        for ( const auto& itemInfo : qskAsConst( m_data->itemInfos ) )
        {
            const auto item = itemInfo.item;

            if ( constraintType( item ) == HeightForWidth )
            {
                const QSizeF hint = QskLayoutConstraint::sizeHint(
                    item, Qt::PreferredSize, constraint );

                height = qMax( height, hint.height() );
            }
        }
    }
#endif

    return QSizeF( width, height );
}

qreal QskStackBox::heightForWidth( qreal width ) const
{
    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::HeightForWidth, this, width, qskConstrainedValue );
}

qreal QskStackBox::widthForHeight( qreal height ) const
{
    return QskLayoutConstraint::constrainedMetric(
        QskLayoutConstraint::WidthForHeight, this, height, qskConstrainedValue );
}

bool QskStackBox::event( QEvent* event )
{
    switch ( static_cast< int >( event->type() ) )
    {
        case QEvent::LayoutRequest:
        {
            resetImplicitSize();
            polish();
            break;
        }
        case QEvent::ContentsRectChange:
        case QskEvent::GeometryChange:
        {
            polish();
            break;
        }
    }

    return Inherited::event( event );
}

#include "moc_QskStackBox.cpp"
