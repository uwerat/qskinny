/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SWIPE_VIEW_H
#define QSK_SWIPE_VIEW_H

#include "QskStackBox.h"

class QskTabBar;
class QskTabButton;

class QSK_EXPORT QskSwipeView : public QskStackBox
{
    Q_OBJECT

    typedef QskStackBox Inherited;

  public:
    QSK_SUBCONTROLS( Panel )

    QskSwipeView( QQuickItem* parent = nullptr );
    ~QskSwipeView() override;

    int duration() const;
    void setDuration( int );

  protected:
    bool gestureFilter( QQuickItem*, QEvent* ) override final;
    void gestureEvent( QskGestureEvent* ) override final;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
