#include "MainItem.h"

#include "DashboardPage.h"
#include "MenuBar.h"
#include "RoomsPage.h"

#include <QskGesture.h>
#include <QskEvent.h>
#include <QskLinearBox.h>
#include <QskStackBoxAnimator.h>

#include <QQuickFramebufferObject>
#include <QGuiApplication>
#include <QQuickWindow>

Cube::Cube( QQuickItem* parent )
    : QskStackBox( false, parent )
{
}

void Cube::startAnimation( Qsk::Direction direction )
{
    using Animator = QskStackBoxAnimator4;

    auto animator = qobject_cast< Animator* >( this->animator() );

    if ( animator == nullptr )
    {
        animator = new Animator( this );
        animator->setEasingCurve( QEasingCurve::InOutQuad );
        animator->setDuration( 1000 );

        setAnimator( animator );
    }

    const auto orientation = ( direction == Qsk::LeftToRight || direction == Qsk::RightToLeft )
            ? Qt::Horizontal : Qt::Vertical;
    animator->setOrientation( orientation );

    const bool inverted = ( direction == Qsk::LeftToRight || direction == Qsk::TopToBottom );
    animator->setInverted( inverted );

    int newIndex = 0;

    switch( direction )
    {
        case Qsk::LeftToRight:
        case Qsk::TopToBottom:
            newIndex = currentIndex() + 1;
            break;
        case Qsk::RightToLeft:
        case Qsk::BottomToTop:
            newIndex = currentIndex() - 1;
            break;
    }

    newIndex %= itemCount();
    if( newIndex < 0 )
        newIndex += itemCount();

    setCurrentIndex( newIndex );
}

MainItem::MainItem( QQuickItem* parent )
    : QskControl( parent )
    , m_cube( new Cube( this ) )
    , m_mainLayout( new QskLinearBox( Qt::Horizontal, m_cube ) )
    , m_otherLayout( new QskLinearBox( Qt::Horizontal, m_cube ) )
{
    setAutoLayoutChildren( true );
    setAcceptedMouseButtons( Qt::LeftButton );
    setFiltersChildMouseEvents( true );

    m_panRecognizer.setOrientations( Qt::Horizontal | Qt::Vertical );
    m_panRecognizer.setMinDistance( 50 );
    m_panRecognizer.setWatchedItem( this );

    m_mainLayout->setSpacing( 0 );

    m_otherLayout->setSpacing( 0 );

    (void) new MenuBar( m_mainLayout );
    (void) new DashboardPage( m_mainLayout );

    (void) new MenuBar( m_otherLayout );
    (void) new RoomsPage( m_otherLayout );

    m_cube->addItem( m_mainLayout );
    m_cube->addItem( m_otherLayout );

    m_cube->setCurrentItem( m_mainLayout );
}

void MainItem::gestureEvent( QskGestureEvent* event )
{
    if( event->gesture()->state() == QskGesture::Finished
            && event->gesture()->type() == QskGesture::Pan )
    {
        auto* panGesture = static_cast< const QskPanGesture* >( event->gesture().get() );

        const auto delta = panGesture->origin() - panGesture->position();

        Qsk::Direction direction;

        if( qAbs( delta.x() ) > qAbs( delta.y() ) )
        {
            direction = ( delta.x() < 0 ) ? Qsk::LeftToRight : Qsk::RightToLeft;
        }
        else
        {
            direction = ( delta.y() < 0 ) ? Qsk::TopToBottom : Qsk::BottomToTop;
        }

        m_cube->startAnimation( direction );
    }
}

bool MainItem::gestureFilter( QQuickItem* item, QEvent* event )
{
    auto& recognizer = m_panRecognizer;

    if( event->type() == QEvent::MouseButtonPress )
    {
        const auto mouseEvent = static_cast< QMouseEvent* >( event );

        if( ( item != this ) || ( recognizer.timeout() < 0 ) )
        {
            if( recognizer.hasProcessedBefore( mouseEvent ) )
            {
                return false;
            }
        }

        recognizer.setTimeout( ( item == this ) ? -1 : 100 );
    }

    return recognizer.processEvent( item, event, false );
}

#include "moc_MainItem.cpp"
