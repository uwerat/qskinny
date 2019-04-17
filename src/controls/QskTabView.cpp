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
    QskTabBar* tabBar = nullptr;
    QskStackBox* stackBox = nullptr;
};

QskTabView::QskTabView( QQuickItem* parent )
    : QskTabView( Qsk::Top, parent )
{
}

QskTabView::QskTabView( Qsk::Position tabPosition, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setPolishOnResize( true );

    m_data->tabBar = new QskTabBar( tabPosition, this );
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

    connect( m_data->tabBar, &QskTabBar::positionChanged,
        this, &QskTabView::tabPositionChanged );
}

QskTabView::~QskTabView()
{
}

const QskTabBar* QskTabView::tabBar() const
{
    return m_data->tabBar;
}

void QskTabView::setTabPosition( Qsk::Position position )
{
    if ( position == tabPosition() )
        return;

    m_data->tabBar->setPosition( position );

    polish();
    update();
}

Qsk::Position QskTabView::tabPosition() const
{
    return m_data->tabBar->position();
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

        /*
            We have to remove the item from the stackBox first. Removing
            the tab then will result in a currentIndexChanged, where the stack
            box will be resynced.
         */
        m_data->stackBox->removeAt( index );
        m_data->tabBar->removeTab( index );

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

int QskTabView::itemIndex( const QQuickItem* item )
{
    return m_data->stackBox->indexOf( item );
}

int QskTabView::buttonIndex( const QskTabButton* button )
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
    const QSizeF boxHint = m_data->stackBox->sizeHint();

    qreal w, h;

    if ( orientation() == Qt::Vertical )
    {
        w = qMax( barHint.width(), boxHint.width() );
        h = barHint.height() + boxHint.height();
    }
    else
    {
        w = barHint.width() + boxHint.width();
        h = qMax( barHint.height(), boxHint.height() );
    }

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
