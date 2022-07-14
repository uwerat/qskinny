#include "MainItem.h"

#include "DashboardPage.h"
#include "MenuBar.h"

#include <QskGesture.h>
#include <QskEvent.h>
#include <QskLinearBox.h>
#include <QskStackBox.h>

#include <QQuickFramebufferObject>
#include <QGuiApplication>
#include <QQuickWindow>

MainItem::MainItem( QQuickItem* parent )
    : QskControl( parent )
    , m_cube( new QskStackBox( false, this ) )
    , m_mainLayout( new QskLinearBox( Qt::Horizontal, m_cube ) )
{
    setAutoLayoutChildren( true );
    setAcceptedMouseButtons( Qt::LeftButton );
    setFiltersChildMouseEvents( true );

    m_panRecognizer.setOrientations( Qt::Horizontal );
    m_panRecognizer.setMinDistance( 50 );
    m_panRecognizer.setWatchedItem( this );

    m_mainLayout->setSpacing( 0 );

    (void) new MenuBar( m_mainLayout );
    (void) new DashboardPage( m_mainLayout );

    m_cube->addItem( m_mainLayout );
}

void MainItem::gestureEvent( QskGestureEvent* event )
{
    if( event->gesture()->state() == QskGesture::Finished )
    {
        // ### here start animation
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
