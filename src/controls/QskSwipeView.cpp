/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSwipeView.h"

#include "QskEvent.h"
#include "QskGesture.h"
#include "QskPanGestureRecognizer.h"
#include "QskStackBoxAnimator.h"

class QskSwipeView::PrivateData
{
  public:
    QskPanGestureRecognizer panRecognizer;
    int panRecognizerTimeout = 100;
    int duration = 500;
};

QSK_SUBCONTROL( QskSwipeView, Panel )

QskSwipeView::QskSwipeView( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setSubcontrolProxy( QskBox::Panel, Panel );

    setFiltersChildMouseEvents( true );

    setAcceptedMouseButtons( Qt::LeftButton );

    m_data->panRecognizer.setWatchedItem( this );
    m_data->panRecognizer.setOrientations( Qt::Horizontal );
    m_data->panRecognizer.setMinDistance( 50 );
}

QskSwipeView::~QskSwipeView()
{
}


int QskSwipeView::duration() const
{
    return m_data->duration;
}

void QskSwipeView::setDuration( int duration )
{
    m_data->duration = duration;
}


bool QskSwipeView::gestureFilter( QQuickItem* item, QEvent* event )
{
    // see QskScrollBox.cpp

    auto& recognizer = m_data->panRecognizer;

    if ( event->type() == QEvent::MouseButtonPress )
    {
        if ( ( item != this ) && ( recognizer.timeout() < 0 ) )
        {
            const auto mouseEvent = static_cast< QMouseEvent* >( event );

            if ( recognizer.hasProcessedBefore( mouseEvent ) )
                return false;
        }

        recognizer.setTimeout( ( item == this ) ? -1 : m_data->panRecognizerTimeout );
    }

    return m_data->panRecognizer.processEvent( item, event );
}

void QskSwipeView::gestureEvent( QskGestureEvent* event )
{
    if( event->gesture()->type() == QskGesture::Pan && event->gesture()->state() == QskGesture::Started )
    {
        const auto gesture = static_cast< const QskPanGesture* >( event->gesture().get() );

        auto animator = dynamic_cast< QskStackBoxAnimator1* >( this->animator() );

        if ( animator == nullptr )
        {
            animator = new QskStackBoxAnimator1( this );
            animator->setOrientation( Qt::Horizontal );
        }

        animator->setDuration( m_data->duration );
        setAnimator( animator );

        const auto direction = ( ( gesture->angle() < 90.0 ) || ( gesture->angle() > 270.0 ) )
            ? Qsk::LeftToRight : Qsk::RightToLeft;

        auto newIndex = ( direction == Qsk::LeftToRight ) ? currentIndex() - 1 : currentIndex() + 1;

        if( newIndex < 0 )
            newIndex += itemCount();

        newIndex %= itemCount();

        setCurrentIndex( newIndex );
    }
    else
    {
        Inherited::gestureEvent( event );
    }

}

#include "moc_QskSwipeView.cpp"
