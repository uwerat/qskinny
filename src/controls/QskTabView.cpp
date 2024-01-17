/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setPolishOnResize( true );

    m_data->tabBar = new QskTabBar( this );

    m_data->stackBox = new QskStackBox( this );
    m_data->stackBox->setObjectName( QStringLiteral( "QskTabViewStackBox" ) );

#if 1
    const auto hint = animationHint( Page );
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

    connect( m_data->tabBar, &QskTabBar::countChanged,
        this, &QskTabView::countChanged );

    connect( m_data->tabBar, &QskTabBar::edgeChanged,
        this, &QskTabView::tabBarEdgeChanged );

    connect( m_data->tabBar, &QskTabBar::autoFitTabsChanged,
        this, &QskTabView::autoFitTabsChanged );
}

QskTabView::~QskTabView()
{
}

QskTabBar* QskTabView::tabBar()
{
    return m_data->tabBar;
}

const QskTabBar* QskTabView::tabBar() const
{
    return m_data->tabBar;
}

void QskTabView::setTabBarEdge( Qt::Edge edge )
{
    m_data->tabBar->setEdge( edge );
}

Qt::Edge QskTabView::tabBarEdge() const
{
    return m_data->tabBar->edge();
}

void QskTabView::setAutoFitTabs( bool on )
{
    m_data->tabBar->setAutoFitTabs( on );
}

bool QskTabView::autoFitTabs() const
{
    return m_data->tabBar->autoFitTabs();
}

Qt::Orientation QskTabView::orientation() const
{
    return qskTransposed( m_data->tabBar->orientation() );
}

int QskTabView::addTab( const QString& text, QQuickItem* page )
{
    return insertTab( -1, text, page );
}

int QskTabView::insertTab( int index, const QString& text, QQuickItem* page )
{
    index = m_data->tabBar->insertTab( index, text );

    if ( page && page->parent() == nullptr )
        page->setParent( this );

    m_data->stackBox->insertItem( index, page );

    return index;
}

void QskTabView::removeTab( int index )
{
    if ( index >= 0 && index < m_data->tabBar->count() )
    {
        QPointer< QQuickItem > page = m_data->stackBox->itemAtIndex( index );

        /*
            We have to remove the page from the stackBox first. Removing
            the tab then will result in a currentIndexChanged, where the stack
            box will be resynced.
         */
        m_data->stackBox->removeAt( index );
        m_data->tabBar->removeTab( index );

        if ( page )
        {
            if ( page->parent() == this )
                delete page;
            else
                page->setParentItem( nullptr );
        }
    }
}

void QskTabView::clear( bool autoDelete )
{
    m_data->tabBar->clear( autoDelete );
    m_data->stackBox->clear( autoDelete );
}

void QskTabView::setTabEnabled( int index, bool enabled )
{
    m_data->tabBar->setTabEnabled( index, enabled );
    if ( auto page = pageAt( index ) )
        page->setEnabled( enabled );
}

bool QskTabView::isTabEnabled( int index ) const
{
    return m_data->tabBar->isTabEnabled( index );
}

QQuickItem* QskTabView::pageAt( int index ) const
{
    return m_data->stackBox->itemAtIndex( index );
}

int QskTabView::pageIndex( const QQuickItem* page )
{
    return m_data->stackBox->indexOf( page );
}

QQuickItem* QskTabView::currentPage() const
{
    return pageAt( currentIndex() );
}

int QskTabView::currentIndex() const
{
    return m_data->tabBar->currentIndex();
}

int QskTabView::count() const
{
    return m_data->tabBar->count();
}

QskAspect::Variation QskTabView::effectiveVariation() const
{
    return m_data->tabBar->effectiveVariation();
}

QSizeF QskTabView::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::MaximumSize )
        return QSizeF();

    const auto& tabBar = m_data->tabBar;
    const auto& stackBox = m_data->stackBox;

    auto barHint = tabBar->sizeConstraint( which );

#if 1
    /*
        How to limit the contribution of the tabbar in a reasonable way ?
        QTabWidget uses 200x200 - what is kind of random. TODO ...
     */
    const qreal minBarSize = 200;
#endif

    QSizeF hint;

    if ( orientation() == Qt::Vertical )
    {
        barHint.setWidth( qMin( barHint.width(), minBarSize ) );

        if ( constraint.width() >= 0.0 )
        {
            qreal w = qMax( constraint.width(), barHint.width() );
            qreal h = stackBox->sizeConstraint( which, QSizeF( w, -1.0 ) ).height();

            hint.rheight() = barHint.height() + h;
        }
        else if ( constraint.height() >= 0.0 )
        {
            qreal h = constraint.height() - barHint.height();
            qreal w = stackBox->sizeConstraint( which, QSizeF( -1.0, h ) ).width();

            hint.rwidth() = qMax( barHint.width(), w );
        }
        else
        {
            const auto boxHint = stackBox->sizeConstraint();

            hint.rwidth() = qMax( barHint.width(), boxHint.width() );
            hint.rheight() = barHint.height() + boxHint.height();
        }
    }
    else
    {
        barHint.setHeight( qMin( barHint.height(), minBarSize ) );

        if ( constraint.width() >= 0.0 )
        {
            qreal w = constraint.width() - barHint.width();
            qreal h = stackBox->sizeConstraint( which, QSizeF( w, -1.0 ) ).height();

            hint.rheight() = qMax( barHint.height(), h );
        }
        else if ( constraint.height() >= 0.0 )
        {
            qreal h = qMax( constraint.height(), barHint.height() );
            qreal w = stackBox->sizeConstraint( which, QSizeF( -1.0, h ) ).width();

            hint.rwidth() = barHint.width() + w;
        }
        else
        {
            const auto boxHint = stackBox->sizeConstraint( which );

            hint.rwidth() = barHint.width() + boxHint.width();
            hint.rheight() = qMax( barHint.height(), boxHint.height() );
        }
    }

    return hint;
}

void QskTabView::setCurrentIndex( int index )
{
    m_data->tabBar->setCurrentIndex( index );
}

bool QskTabView::event( QEvent* event )
{
    if ( event->type() == QEvent::LayoutRequest )
    {
        resetImplicitSize();
        polish();
        update();
    }

    return Inherited::event( event );
}

void QskTabView::updateLayout()
{
    if ( maybeUnresized() )
        return;

    const auto cr = contentsRect();

    m_data->tabBar->setGeometry( subControlRect( cr, TabBar ) );
    m_data->stackBox->setGeometry( subControlContentsRect( cr, Page ) );
}

#if 1

/*
  QskTabBar will do scene graph node composition and QskTabButton will go away
  see: https://github.com/uwerat/qskinny/issues/283
 */

int QskTabView::addTab( QskTabButton* button, QQuickItem* page )
{
    return insertTab( -1, button, page );
}

int QskTabView::insertTab( int index, QskTabButton* button, QQuickItem* page )
{
    if ( page && page->parent() == nullptr )
        page->setParent( this );

    index = m_data->tabBar->insertTab( index, button );
    m_data->stackBox->insertItem( index, page );

    return index;
}

QskTabButton* QskTabView::buttonAt( int index ) const
{
    return m_data->tabBar->buttonAt( index );
}

int QskTabView::buttonIndex( const QskTabButton* button )
{
    return m_data->tabBar->indexOf( button );
}

QskTabButton* QskTabView::currentButton() const
{
    return buttonAt( currentIndex() );
}

#endif

#include "moc_QskTabView.cpp"
