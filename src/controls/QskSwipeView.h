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

    Q_PROPERTY( int duration READ duration WRITE setDuration RESET resetDuration )

    using Inherited = QskStackBox;

  public:
    QSK_SUBCONTROLS( Panel )

    QskSwipeView( QQuickItem* parent = nullptr );
    ~QskSwipeView() override;

    int duration() const;
    void setDuration( int );
    void resetDuration();

    QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol ) const;

  protected:
    bool gestureFilter( const QQuickItem*, const QEvent* ) override;
    void gestureEvent( QskGestureEvent* ) override;

  private:
    void setAnimator( QskStackBoxAnimator* ) = delete;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
