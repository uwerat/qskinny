/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabView.h"
#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskStackBox.h"
#include "QskStackBoxAnimator.h"
#include "QskTabBar.h"
#include "QskTabButton.h"

#include <QPointer>

QSK_SUBCONTROL( QskTabView, TabBar )
QSK_SUBCONTROL( QskTabView, Page )

static inline Qt::Orientation qskTransposed( Qt::Orientation orientation )
{
    return ( orientation == Qt::Vertical ) ? Qt::Horizontal : Qt::Vertical;
}

class QskTabView::PrivateData
{
  public:
    PrivateData()
        : tabBar( nullptr )
        , stackBox( nullptr )
    {
    }

    QskTabBar* tabBar;
    QskStackBox* stackBox;
};

QskTabView::QskTabView( QQuickItem* parent )
    : QskTabView( Qt::Vertical, parent )
{
}

QskTabView::QskTabView( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setPolishOnResize( true );

    m_data->tabBar = new QskTabBar( qskTransposed( orientation ), this );
    m_data->tabBar->setZ( 1 );

    m_data->stackBox = new QskStackBox( this );
    m_data->stackBox->setObjectName( QStringLiteral( "QskTabViewStackBox" ) );
    m_data->stackBox->setZ( 0 );

#if 1
    const QskAnimationHint hint = animation( Page );
    if ( hint.duration > 0 )
    {
        // When changing the skin, we have to update the animator. TODO ...

        auto animator = new QskStackBoxAnimator3( m_data->stackBox );
        animator->setDuration( hint.duration );
        animator->setEasingCurve( hint.type );

        m_data->stackBox->setAnimator( animator );
    }
#endif

    connect( m_data->tabBar, &QskTabBar::currentIndexChanged,
        m_data->stackBox, &QskStackBox::setCurrentIndex );

    connect( m_data->tabBar, &QskTabBar::currentIndexChanged,
        this, &QskTabView::currentIndexChanged );
}

QskTabView::~QskTabView()
{
}

const QskTabBar* QskTabView::tabBar() const
{
    return m_data->tabBar;
}

void QskTabView::setOrientation( Qt::Orientation orientation )
{
    const Qt::Orientation o = qskTransposed( orientation );
    if ( o != m_data->tabBar->orientation() )
    {
        m_data->tabBar->setOrientation( o );

        polish();
        update();

        Q_EMIT orientationChanged();
    }
}

Qt::Orientation QskTabView::orientation() const
{
    return qskTransposed( m_data->tabBar->orientation() );
}

int QskTabView::addTab( QskTabButton* button, QQuickItem* item )
{
    return insertTab( -1, button, item );
}

int QskTabView::insertTab( int index, QskTabButton* button, QQuickItem* item )
{
    // multiple insertion ???

    if ( item && item->parent() == nullptr )
        item->setParent( this );

    index = m_data->tabBar->insertTab( index, button );
    m_data->stackBox->insertItem( index, item, Qt::Alignment() );

    return index;
}

int QskTabView::addTab( const QString& tabText, QQuickItem* item )
{
    return insertTab( -1, tabText, item );
}

int QskTabView::insertTab( int index, const QString& tabText, QQuickItem* item )
{
    return insertTab( index, new QskTabButton( tabText ), item );
}

void QskTabView::removeTab( int index )
{
    if ( index >= 0 && index < m_data->tabBar->count() )
    {
        QPointer< QQuickItem > tabItem = m_data->stackBox->itemAtIndex( index );

        // indexes still in sync ???

        m_data->tabBar->removeTab( index );
        m_data->stackBox->removeAt( index );

        if ( tabItem )
        {
            if ( tabItem->parent() == this )
                delete tabItem;
            else
                tabItem->setParentItem( nullptr );
        }
    }
}

void QskTabView::clear()
{
    m_data->tabBar->clear();
    m_data->stackBox->clear();
}

QskTabButton* QskTabView::buttonAt( int index ) const
{
    return m_data->tabBar->buttonAt( index );
}

QQuickItem* QskTabView::itemAt( int index ) const
{
    return m_data->stackBox->itemAtIndex( index );
}

int QskTabView::itemIndex( QQuickItem* item )
{
    return m_data->stackBox->indexOf( item );
}

int QskTabView::buttonIndex( QskTabButton* button )
{
    return m_data->tabBar->indexOf( button );
}

QQuickItem* QskTabView::currentItem() const
{
    return itemAt( currentIndex() );
}

QskTabButton* QskTabView::currentButton() const
{
    return buttonAt( currentIndex() );
}

int QskTabView::currentIndex() const
{
    return m_data->tabBar->currentIndex();
}

int QskTabView::count() const
{
    return m_data->tabBar->count();
}

QSizeF QskTabView::contentsSizeHint() const
{
    if ( m_data->tabBar == nullptr || m_data->tabBar->count() == 0 )
        return Inherited::contentsSizeHint();

    const QSizeF barHint = m_data->tabBar->sizeHint();
    const QSizeF itemHint = m_data->stackBox->sizeHint();

    const qreal w = qMax( barHint.width(), itemHint.width() );
    const qreal h = barHint.height() + itemHint.height();

    return QSizeF( w, h );
}

void QskTabView::setCurrentIndex( int index )
{
    m_data->tabBar->setCurrentIndex( index );
}

bool QskTabView::event( QEvent* event )
{
    switch ( event->type() )
    {
        case QEvent::LayoutRequest:
        {
            resetImplicitSize();
            polish();
            break;
        }
        default:
            break;
    }

    return Inherited::event( event );
}

void QskTabView::updateLayout()
{
    m_data->tabBar->setGeometry( subControlRect( TabBar ) );
    m_data->stackBox->setGeometry( subControlRect( Page ) );
}

#include "moc_QskTabView.cpp"
