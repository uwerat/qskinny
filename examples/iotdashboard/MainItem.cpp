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

QPair< Cube::Position, Cube::Edge > Cube::s_neighbors[ Cube::NumPositions ][ Cube::NumEdges ] =
{
    // neighbors of Left side:
    {
        { Cube::BackPos, Cube::BottomEdge }, // going Left
        { Cube::FrontPos, Cube::BottomEdge }, // going Right
        { Cube::TopPos, Cube::LeftEdge }, // going Top
        { Cube::BottomPos, Cube::RightEdge } // going Bottom
    },

    // Right:
    {
        { Cube::FrontPos, Cube::BottomEdge },
        { Cube::BackPos, Cube::BottomEdge },
        { Cube::TopPos, Cube::RightEdge },
        { Cube::BottomPos, Cube::LeftEdge }
    },

    // Top:
    {
        { Cube::LeftPos, Cube::RightEdge },
        { Cube::RightPos, Cube::LeftEdge },
        { Cube::BackPos, Cube::TopEdge },
        { Cube::FrontPos, Cube::BottomEdge }
    },

    // Bottom:
    {
        { Cube::LeftPos, Cube::LeftEdge },
        { Cube::RightPos, Cube::RightEdge },
        { Cube::FrontPos, Cube::BottomEdge },
        { Cube::BackPos, Cube::TopEdge }
    },

    // Front:
    {
        { Cube::LeftPos, Cube::BottomEdge },
        { Cube::RightPos, Cube::BottomEdge },
        { Cube::TopPos, Cube::BottomEdge },
        { Cube::BottomPos, Cube::BottomEdge }
    },

    // Back:
    {
        { Cube::RightPos, Cube::BottomEdge },
        { Cube::LeftPos, Cube::BottomEdge },
        { Cube::TopPos, Cube::TopEdge },
        { Cube::BottomPos, Cube::BottomEdge }
    }
};

Cube::Edge Cube::s_edgeTransformations[ Cube::NumEdges ][ Cube::NumEdges ] =
{
    // current edge is LeftEdge:
    { Cube::TopEdge, // Left
      Cube::BottomEdge, // Right
      Cube::RightEdge, // Top
      Cube::LeftEdge }, // Bottom

    // Right:
    { Cube::BottomEdge,
      Cube::TopEdge,
      Cube::LeftEdge,
      Cube::RightEdge },

    // Top:
    { Cube::RightEdge,
      Cube::LeftEdge,
      Cube::BottomEdge,
      Cube::TopEdge },

    // Bottom:
    { Cube::LeftEdge,
      Cube::RightEdge,
      Cube::TopEdge,
      Cube::BottomEdge }
};

Cube::Cube( QQuickItem* parent )
    : QskStackBox( false, parent )
    , m_destination( FrontPos )
    , m_currentEdge( BottomEdge )
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

    QTimer::singleShot( 0, this, [this]()
    {
        Q_EMIT cubeIndexChanged( m_destination );
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

    const auto orientation = ( direction == Qsk::LeftToRight || direction == Qsk::RightToLeft )
            ? Qt::Horizontal : Qt::Vertical;
    animator->setOrientation( orientation );

    const bool inverted = ( direction == Qsk::LeftToRight || direction == Qsk::TopToBottom );
    animator->setInverted( inverted );

    updateEdge( direction, position );

    setCurrentIndex( position );

    if( position == m_destination )
    {
        Q_EMIT cubeIndexChanged( position );
    }
}

void Cube::switchPosition( const Qsk::Direction direction )
{
    m_destination = neighbor( currentPosition(), direction );

    doSwitch( direction, m_destination );
}

void Cube::switchToPosition( const Position position )
{
    if( currentPosition() == position )
        return;

    m_destination = position;

    const auto direction = this->direction( currentPosition(), position );
    const auto nextPosition = neighbor( currentPosition(), direction );

    doSwitch( direction, nextPosition );
}

void Cube::keyPressEvent( QKeyEvent* event )
{
    Qsk::Direction direction;

    switch( event->key() )
    {
    case Qt::Key_Up:
        direction = Qsk::TopToBottom;
        break;
    case Qt::Key_Down:
        direction = Qsk::BottomToTop;
        break;
    case Qt::Key_Left:
        direction = Qsk::LeftToRight;
        break;
    case Qt::Key_Right:
        direction = Qsk::RightToLeft;
        break;
    default:
        return;
    }

    switchPosition( direction );
}

Cube::Position Cube::currentPosition() const
{
    return static_cast< Position >( currentIndex() );
}

Cube::Position Cube::neighbor( const Position position, const Qsk::Direction direction ) const
{
    const auto index = s_edgeTransformations[ m_currentEdge ][ direction ];
    const auto n = s_neighbors[ position ][ index ].first;
    return n;
}

Qsk::Direction Cube::direction( const Position from, const Position to ) const
{
    // if direct neighbor: use that direction
    // otherwise: we need 2 swipes, direction doesn't matter, so choose right to left

    const auto neighbors = s_neighbors[ from ];

    for( int i = 0; i < NumEdges; ++i )
    {
        if( neighbors[ i ].first == to )
        {
            return static_cast< Qsk::Direction >( i );
        }
    }

    return Qsk::RightToLeft;
}

void Cube::updateEdge( Qsk::Direction direction, Position position )
{
    m_currentEdge = s_neighbors[ currentPosition() ][ direction ].second;

    // When going back to Front, Left etc., switch back to
    // the bottom edge, otherwise it gets to confusing:
    if( position != TopPos && position != BottomPos )
    {
        m_currentEdge = BottomEdge;
    }
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

    connect( m_cube, &Cube::cubeIndexChanged, m_menuBar, &MenuBar::setActivePage );

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

    installEventFilter( this );
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

bool MainItem::eventFilter( QObject* object, QEvent* event )
{
    if ( event->type() == QEvent::KeyPress )
    {
        QCoreApplication::sendEvent( m_cube, event );
        return true;
    }
    else
    {
        return QObject::eventFilter( object, event );
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
