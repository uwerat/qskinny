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

    // should be skin hints
    recognizer.setOrientations( Qt::Horizontal );
    recognizer.setMinDistance( 50 );
    recognizer.setTimeout( 100 );

    resetDuration();
}

QskSwipeView::~QskSwipeView()
{
}

QskAspect::Subcontrol QskSwipeView::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskSwipeView::Panel;

    return Inherited::effectiveSubcontrol( subControl );
}

int QskSwipeView::duration() const
{
    return m_data->duration;
}

void QskSwipeView::setDuration( int duration )
{
    m_data->duration = duration;
}

void QskSwipeView::resetDuration()
{
    m_data->duration = 500;
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
            animator->setOrientation( Qt::Horizontal );
        }

        animator->setDuration( m_data->duration );
        QskStackBox::setAnimator( animator );

        const auto direction = ( ( gesture->angle() < 90.0 ) || ( gesture->angle() > 270.0 ) )
            ? Qsk::LeftToRight : Qsk::RightToLeft;

        auto newIndex = ( direction == Qsk::LeftToRight ) ? currentIndex() - 1 : currentIndex() + 1;

        if( newIndex < 0 )
            newIndex += itemCount();

        newIndex %= itemCount();

        setCurrentIndex( newIndex );
        return;
    }

    Inherited::gestureEvent( event );
}

#include "moc_QskSwipeView.cpp"
