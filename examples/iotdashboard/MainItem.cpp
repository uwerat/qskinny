#include "MainItem.h"

#include "DashboardPage.h"
#include "DevicesPage.h"
#include "MenuBar.h"
#include "MembersPage.h"
#include "RoomsPage.h"
#include "StatisticsPage.h"
#include "StoragePage.h"

#include <QskGesture.h>
#include <QskEvent.h>
#include <QskLinearBox.h>
#include <QskStackBoxAnimator.h>

#include <QQuickFramebufferObject>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QtMath>

#include <QTimer>

Cube::Position Cube::s_neighbors[ Cube::NumPositions ][ 4 ] =
{
    // Left:
    { Cube::BackPos, // Left
      Cube::FrontPos, // Right
      Cube::TopPos, // Top
      Cube::BottomPos }, // Bottom

    // Right:
    { Cube::FrontPos,
      Cube::BackPos,
      Cube::TopPos,
      Cube::BottomPos },

    // Top:
    { Cube::LeftPos,
      Cube::RightPos,
      Cube::BackPos,
      Cube::FrontPos },

    // Bottom:
    { Cube::LeftPos,
      Cube::RightPos,
      Cube::FrontPos,
      Cube::BackPos },

    // Front:
    { Cube::LeftPos,
      Cube::RightPos,
      Cube::TopPos,
      Cube::BottomPos },

    // Back:
    { Cube::RightPos,
      Cube::LeftPos,
      Cube::TopPos,
      Cube::BottomPos },
};

Cube::Cube( QQuickItem* parent )
    : QskStackBox( false, parent )
    , m_destination( FrontPos )
    , m_previousPosition( FrontPos )
    , m_isIntermediateHop( false )
{
    // The code below covers the case where we need 2 cube movements to get
    // to the desired position.
    // We use transientIndexChanged here to be sure to start a new transition
    // at the end; indexChanged doesn't work here.

    connect( this, &QskStackBox::transientIndexChanged, this, [ this ]( qreal position )
    {
        const bool animationIsFinished = ( position == qFloor( position ) );

        if( animationIsFinished && position != m_destination )
        {
            QTimer::singleShot( 0, this, [this]()
            {
                m_isIntermediateHop = true;
                switchToPosition( m_destination );
            } );
        }
    } );
}

void Cube::doSwitch( Qsk::Direction direction, Position position )
{
    using Animator = QskStackBoxAnimator4;

    auto animator = qobject_cast< Animator* >( this->animator() );

    if ( animator == nullptr )
    {
        animator = new Animator( this );
        animator->setDuration( 1000 );
        setAnimator( animator );
    }

    if( position == m_destination && !m_isIntermediateHop ) // 1 hop
    {
        animator->setEasingCurve( QEasingCurve::InOutQuad );
    }
    else if( !m_isIntermediateHop ) // 1st of 2 hops
    {
        animator->setEasingCurve( QEasingCurve::InQuad );
    }
    else // 2nd of 2 hops
    {
        animator->setEasingCurve( QEasingCurve::OutQuad );
        m_isIntermediateHop = false;
    }

    m_previousPosition = m_destination;

    const auto orientation = ( direction == Qsk::LeftToRight || direction == Qsk::RightToLeft )
            ? Qt::Horizontal : Qt::Vertical;
    animator->setOrientation( orientation );

    const bool inverted = ( direction == Qsk::LeftToRight || direction == Qsk::TopToBottom );
    animator->setInverted( inverted );

    setCurrentIndex( position );
    Q_EMIT cubeIndexChanged( position );
}

void Cube::switchPosition( const Qsk::Direction direction )
{
    // keep track of from where we went to top and bottom,
    // so that going up and down will result in going back
    // to the same position:
    // (We don't want to model the complete cube logic with
    // keeping track of the edges here, because that doesn't
    // make sense wrt. being upside down etc.)

    if( ( m_destination == TopPos && direction == Qsk::BottomToTop )
            || ( m_destination == BottomPos && direction == Qsk::TopToBottom ) )
    {
        m_destination = m_previousPosition; // ### doesn't work completely yet
    }
    else
    {
        m_destination = neighbor( m_destination, direction );
    }

    doSwitch( direction, m_destination );
}

void Cube::switchToPosition( const Position position )
{
    if( currentPosition() == position )
        return;

    m_destination = position;

    const auto from = static_cast< Position >( currentIndex() );
    const auto direction = this->direction( from, position );
    const auto intermediatePosition = neighbor( currentPosition(), direction );

    doSwitch( direction, intermediatePosition );
}

Cube::Position Cube::currentPosition() const
{
    return static_cast< Position >( currentIndex() );
}

Cube::Position Cube::neighbor( const Position position, const Qsk::Direction direction ) const
{
    const auto n = s_neighbors[ position ][ direction ];
    return n;
}

Qsk::Direction Cube::direction( const Position from, const Position to ) const
{
    // if direct neighbor: use that direction
    // otherwise: we need 2 swipes, direction doesn't matter, so choose right to left

    const auto neighbors = s_neighbors[ from ];

    for( int i = 0; i < 4; ++i )
    {
        if( neighbors[ i ] == to )
        {
            return static_cast< Qsk::Direction >( i );
        }
    }

    return Qsk::RightToLeft;
}

MainItem::MainItem( QQuickItem* parent )
    : QskControl( parent )
    , m_mainLayout( new QskLinearBox( Qt::Horizontal, this ) )
    , m_menuBar( new MenuBar( m_mainLayout ) )
    , m_cube( new Cube( m_mainLayout ) )
{
    setAutoLayoutChildren( true );
    setAcceptedMouseButtons( Qt::LeftButton );
    setFiltersChildMouseEvents( true );

    m_panRecognizer.setOrientations( Qt::Horizontal | Qt::Vertical );
    m_panRecognizer.setMinDistance( 50 );
    m_panRecognizer.setWatchedItem( this );

    m_mainLayout->setSpacing( 0 );

    connect( m_menuBar, &MenuBar::pageChangeRequested, this, [this]( int index )
    {
        const auto position = static_cast< Cube::Position >( index );
        m_cube->switchToPosition( position );
    } );

    auto* const dashboardPage = new DashboardPage( m_cube );
    auto* const roomsPage = new RoomsPage( m_cube );
    auto* const devicesPage = new DevicesPage( m_cube );
    auto* const statisticsPage = new StatisticsPage( m_cube );
    auto* const storagePage = new StoragePage( m_cube );
    auto* const membersPage = new MembersPage( m_cube );

    m_cube->insertItem( Cube::LeftPos, statisticsPage );
    m_cube->insertItem( Cube::RightPos, roomsPage );
    m_cube->insertItem( Cube::TopPos, storagePage );
    m_cube->insertItem( Cube::BottomPos, membersPage );
    m_cube->insertItem( Cube::FrontPos, dashboardPage );
    m_cube->insertItem( Cube::BackPos, devicesPage );

    // the current item needs to be the one at the Front:
    m_cube->setCurrentItem( dashboardPage );
}

void MainItem::gestureEvent( QskGestureEvent* event )
{
    if( event->gesture()->state() == QskGesture::Finished
            && event->gesture()->type() == QskGesture::Pan )
    {
        const auto* panGesture = static_cast< const QskPanGesture* >( event->gesture().get() );

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

        m_cube->switchPosition( direction );
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
