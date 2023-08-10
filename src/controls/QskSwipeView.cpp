/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSwipeView.h"

#include "QskEvent.h"
#include "QskGesture.h"
#include "QskPanGestureRecognizer.h"
#include "QskStackBoxAnimator.h"
#include "QskPlatform.h"

class QskSwipeView::PrivateData
{
  public:
    QskPanGestureRecognizer panRecognizer;
    int duration = -1; // should be a skinHint
};

QSK_SUBCONTROL( QskSwipeView, Panel )

QskSwipeView::QskSwipeView( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setFiltersChildMouseEvents( true );
    setAcceptedMouseButtons( Qt::LeftButton );

    auto& recognizer = m_data->panRecognizer;

    recognizer.setWatchedItem( this );

    // should be skin hints, TODO
    recognizer.setOrientations( Qt::Horizontal );
    recognizer.setTimeout( 100 );

    resetSwipeDistance();
    resetDuration();
}

QskSwipeView::~QskSwipeView()
{
}

void QskSwipeView::setOrientation( Qt::Orientation orientation )
{
    if ( orientation != this->orientation() )
    {
        m_data->panRecognizer.setOrientations( orientation );
        Q_EMIT orientationChanged( orientation );
    }
}

Qt::Orientation QskSwipeView::orientation() const
{
    return ( m_data->panRecognizer.orientations() == Qt::Vertical )
        ? Qt::Vertical : Qt::Horizontal;
}

int QskSwipeView::swipeDistance() const
{
    return m_data->panRecognizer.minDistance();
}

void QskSwipeView::setSwipeDistance( int distance )
{
    const auto oldDistance = m_data->panRecognizer.minDistance();
    m_data->panRecognizer.setMinDistance( distance );

    if ( oldDistance != m_data->panRecognizer.minDistance() )
        Q_EMIT swipeDistanceChanged( m_data->panRecognizer.minDistance() );
}

void QskSwipeView::resetSwipeDistance()
{
    setSwipeDistance( qRound( qskDpToPixels( 40 ) ) );
}

int QskSwipeView::duration() const
{
    return m_data->duration;
}

void QskSwipeView::setDuration( int duration )
{
    if ( duration != m_data->duration )
    {
        m_data->duration = duration;
        Q_EMIT durationChanged( duration );
    }
}

void QskSwipeView::resetDuration()
{
    setDuration( 500 );
}

bool QskSwipeView::gestureFilter( const QQuickItem* item, const QEvent* event )
{
    // see QskScrollBox.cpp

    auto& recognizer = m_data->panRecognizer;

    if ( event->type() == QEvent::MouseButtonPress )
    {
        auto mouseEvent = static_cast< const QMouseEvent* >( event );
        if ( recognizer.hasProcessedBefore( mouseEvent ) )
            return false;
    }

    return recognizer.processEvent( item, event );

}

void QskSwipeView::gestureEvent( QskGestureEvent* event )
{
    const auto gesture = static_cast< const QskPanGesture* >( event->gesture().get() );

    if( gesture->type() == QskGesture::Pan && gesture->state() == QskGesture::Started )
    {
        auto animator = dynamic_cast< QskStackBoxAnimator1* >( this->animator() );

        if ( animator == nullptr )
        {
            animator = new QskStackBoxAnimator1( this );
            animator->setOrientation( orientation() );
        }

        animator->setDuration( m_data->duration );
        QskStackBox::setAnimator( animator );

        bool forwards;

        if ( orientation() == Qt::Horizontal )
            forwards = gesture->angle() >= 90.0 && gesture->angle() <= 270.0;
        else
            forwards = gesture->angle() >= 180.0;

        auto newIndex = forwards ? currentIndex() + 1 : currentIndex() - 1;

        if( newIndex < 0 )
            newIndex += itemCount();

        newIndex %= itemCount();

        setCurrentIndex( newIndex );
        return;
    }

    Inherited::gestureEvent( event );
}

QskAspect::Subcontrol QskSwipeView::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskSwipeView::Panel;

    return Inherited::effectiveSubcontrol( subControl );
}


#include "moc_QskSwipeView.cpp"
