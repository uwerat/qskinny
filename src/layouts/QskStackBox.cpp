/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStackBox.h"
#include "QskStackBoxAnimator.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <QPointer>

class QskStackBox::PrivateData
{
  public:
    QVector< QQuickItem* > items;
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
    return m_data->items.count();
}

QQuickItem* QskStackBox::itemAtIndex( int index ) const
{
    return m_data->items.value( index );
}

int QskStackBox::indexOf( const QQuickItem* item ) const
{
    if ( item && ( item->parentItem() == this ) )
    {
        for ( int i = 0; i < m_data->items.count(); i++ )
        {
            if ( item == m_data->items[i] )
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
    polish();

    Q_EMIT currentIndexChanged( m_data->currentIndex );
}

void QskStackBox::setCurrentItem( const QQuickItem* item )
{
    setCurrentIndex( indexOf( item ) );
}

void QskStackBox::addItem( QQuickItem* item )
{
    insertItem( -1, item );
}

void QskStackBox::addItem( QQuickItem* item, Qt::Alignment alignment )
{
    insertItem( -1, item, alignment );
}

void QskStackBox::insertItem( int index, QQuickItem* item )
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
                return;
            }

            m_data->items.removeAt( oldIndex );
        }
    }

    if ( doAppend )
        index = itemCount();

    m_data->items.insert( index, item );

    const int oldCurrentIndex = m_data->currentIndex;

    if ( m_data->items.count() == 1 )
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

void QskStackBox::insertItem(
    int index, QQuickItem* item, Qt::Alignment alignment )
{
    if ( auto control = qskControlCast( item ) )
        control->setLayoutAlignmentHint( alignment );

    insertItem( index, item );
}

void QskStackBox::removeAt( int index )
{
    removeItemInternal( index, true );
}

void QskStackBox::removeItemInternal( int index, bool unparent )
{
    if ( index < 0 || index >= m_data->items.count() )
        return;

    if ( unparent )
    {
        if ( auto item = m_data->items[ index ] )
            unparentItem( item );
    }

    m_data->items.removeAt( index );

    auto& currentIndex = m_data->currentIndex;

    if ( index <= currentIndex )
    {
        currentIndex--;

        if ( currentIndex < 0 && !m_data->items.isEmpty() )
            currentIndex = 0;

        if ( currentIndex >= 0 )
            m_data->items[ currentIndex ]->setVisible( true );

        Q_EMIT currentIndexChanged( currentIndex );
    }

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
    for ( const auto item : qskAsConst( m_data->items ) )
    {
        if( autoDelete && ( item->parent() == this ) )
            delete item;
        else
            item->setParentItem( nullptr );
    }

    m_data->items.clear();

    if ( m_data->currentIndex >= 0 )
    {
        m_data->currentIndex = -1;
        Q_EMIT currentIndexChanged( m_data->currentIndex );
    }
}

QRectF QskStackBox::geometryForItemAt( int index ) const
{
    const auto r = layoutRect();

    if ( const auto item = m_data->items.value( index ) )
    {
        auto alignment = qskLayoutAlignmentHint( item );
        if ( alignment == 0 )
            alignment = m_data->defaultAlignment;

        return qskConstrainedItemRect( item, r, alignment );
    }

    return QRectF( r.x(), r.y(), 0.0, 0.0 );
}

void QskStackBox::updateLayout()
{
    if ( maybeUnresized() )
        return;

#if 1
    // what about QskControl::LayoutOutWhenHidden
#endif

    const auto index = m_data->currentIndex;

    if ( index >= 0 )
    {
        const auto rect = geometryForItemAt( index );
        qskSetItemGeometry( m_data->items[ index ], rect );
    }
}

QSizeF QskStackBox::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::MaximumSize )
        return QSizeF();

    qreal w = -1.0;
    qreal h = -1.0;

    for ( const auto item : m_data->items )
    {
        /*
            We ignore the retainSizeWhenVisible flag and include all
            invisible items. Maybe we should offer a flag to control this ?
         */
        const auto policy = qskSizePolicy( item );

        if ( constraint.width() >= 0.0 && policy.isConstrained( Qt::Vertical ) )
        {
            const auto hint = qskSizeConstraint( item, which, constraint );
            h = qMax( h, hint.height() );
        }
        else if ( constraint.height() >= 0.0 && policy.isConstrained( Qt::Horizontal ) )
        {
            const auto hint = qskSizeConstraint( item, which, constraint );
            w = qMax( w, hint.width() );
        }
        else
        {
            const auto hint = qskSizeConstraint( item, which, QSizeF() );

            w = qMax( w, hint.width() );
            h = qMax( h, hint.height() );
        }
    }

    // minimum layout hint needs to be cached TODO ...
    return QSizeF( w, h );
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

void QskStackBox::dump() const
{
    auto debug = qDebug();

    QDebugStateSaver saver( debug );
    debug.nospace();

    const auto constraint = sizeConstraint();

    debug << "QskStackBox"
        << " w:" << constraint.width() << " h:" << constraint.height() << '\n';

    for ( int i = 0; i < m_data->items.count(); i++ )
    {
        const auto item = m_data->items[i];

        debug << "  " << i << ": ";

        const auto constraint = qskSizeConstraint( item, Qt::PreferredSize );
        debug << item->metaObject()->className()
            <<  " w:" << constraint.width() << " h:" << constraint.height();

        if ( i == m_data->currentIndex )
            debug << " [X]";

        debug << '\n';
    }
}

#include "moc_QskStackBox.cpp"
