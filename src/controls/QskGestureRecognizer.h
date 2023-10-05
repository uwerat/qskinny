/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GESTURE_RECOGNIZER_H
#define QSK_GESTURE_RECOGNIZER_H

#include "QskGlobal.h"

#include <qobject.h>
#include <qnamespace.h>
#include <memory>

class QQuickItem;
class QEvent;
class QMouseEvent;

class QSK_EXPORT QskGestureRecognizer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( State state READ state NOTIFY stateChanged )
    Q_PROPERTY( QQuickItem* watchedItem READ watchedItem WRITE setWatchedItem )

    Q_PROPERTY( Qt::MouseButtons acceptedMouseButtons
        READ acceptedMouseButtons WRITE setAcceptedMouseButtons )

    Q_PROPERTY( int timeout READ timeout WRITE setTimeout )

    using Inherited = QObject;

  public:
    enum State
    {
        Idle,
        Pending,
        Accepted
    };

    Q_ENUM( State )

    QskGestureRecognizer( QObject* parent = nullptr );
    ~QskGestureRecognizer() override;

    bool eventFilter( QObject* object, QEvent* event) override;

    void setWatchedItem( QQuickItem* );
    QQuickItem* watchedItem() const;

    // Qt::NoButton means: all buttons accepted
    void setAcceptedMouseButtons( Qt::MouseButtons );
    Qt::MouseButtons acceptedMouseButtons() const;

    void setRejectOnTimeout( bool );
    bool rejectOnTimeout() const;

    void setTimeout( int );
    int timeout() const;

    // timestamp, when the Idle state had been left
    quint64 timestampStarted() const;

    void reject();
    void accept();
    void abort();

    State state() const;

    virtual QRectF gestureRect() const;

  Q_SIGNALS:
    void stateChanged( State from, State to );

  protected:
    void timerEvent( QTimerEvent* ) override;

    /*
        This API works for single-touch gestures and multi-touch gestures,
        that can be translated to single positions ( f.e 2 finger swipes ).
        Once we support more complex inputs ( f.e pinch gesture ) we need to
        make substantial adjustments here.
     */
    virtual void processPress( const QPointF&, quint64 timestamp, bool isFinal );
    virtual void processMove( const QPointF&, quint64 timestamp );
    virtual void processRelease( const QPointF&, quint64 timestamp );

  private:
    bool maybeGesture( const QQuickItem*, const QEvent* );
    bool processMouseEvent( const QQuickItem*, const QMouseEvent* );

    void setState( State );
    void reset();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
