/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GESTURE_RECOGNIZER_H
#define QSK_GESTURE_RECOGNIZER_H

#include "QskGlobal.h"

#include <qnamespace.h>
#include <memory>

class QQuickItem;
class QEvent;
class QMouseEvent;

class QSK_EXPORT QskGestureRecognizer
{
  public:
    enum State
    {
        Idle,
        Pending,
        Accepted
    };

    QskGestureRecognizer();
    virtual ~QskGestureRecognizer();

    void setWatchedItem( QQuickItem* );
    QQuickItem* watchedItem() const;

    // Qt::NoButton means: all buttons accepted
    void setAcceptedMouseButtons( Qt::MouseButtons );
    Qt::MouseButtons acceptedMouseButtons() const;

    void setTimeout( int );
    int timeout() const;

    // timestamp, when the Idle state had been left
    ulong timestamp() const;

    bool processEvent( QQuickItem*, QEvent*, bool blockReplayedEvents = true );

    void reject();
    void accept();
    void abort();

    State state() const;

    bool isReplaying() const;
    bool hasProcessedBefore( const QMouseEvent* ) const;

  protected:
    virtual void pressEvent( const QMouseEvent* );
    virtual void moveEvent( const QMouseEvent* );
    virtual void releaseEvent( const QMouseEvent* );

    virtual void stateChanged( State from, State to );

  private:
    void setState( State );
    void reset();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
