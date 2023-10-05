/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SWIPE_VIEW_H
#define QSK_SWIPE_VIEW_H

#include "QskStackBox.h"

class QSK_EXPORT QskSwipeView : public QskStackBox
{
    Q_OBJECT

    Q_PROPERTY( int duration READ duration
        WRITE setDuration RESET resetDuration NOTIFY durationChanged )

    Q_PROPERTY( int swipeDistance READ swipeDistance
        WRITE setSwipeDistance RESET resetSwipeDistance NOTIFY swipeDistanceChanged )

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    using Inherited = QskStackBox;

  public:
    QSK_SUBCONTROLS( Panel )

    QskSwipeView( QQuickItem* parent = nullptr );
    ~QskSwipeView() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    // Duration is the time ( in ms ) used for changing between pages

    int duration() const;
    void setDuration( int );
    void resetDuration();

    /*
        Even if called "swipe view" we use a pan - no swipe - gesture.
        ( = pages are moved before the gesture has been confirmed )

        The swipe distance is the minimum distance in pixels of the pan gesture
     */

    int swipeDistance() const;
    void setSwipeDistance( int );
    void resetSwipeDistance();

    QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol ) const;

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void durationChanged( int );
    void swipeDistanceChanged( int );

  protected:
    void gestureEvent( QskGestureEvent* ) override;

  private:
    void setAnimator( QskStackBoxAnimator* ) = delete;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
