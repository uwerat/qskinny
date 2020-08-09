/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ANIMATOR_H
#define QSK_ANIMATOR_H

#include "QskGlobal.h"

#include <qeasingcurve.h>
#include <qobjectdefs.h>

class QQuickWindow;
class QObject;
class QDebug;

class QSK_EXPORT QskAnimator
{
  public:
    QskAnimator();
    virtual ~QskAnimator();

    QQuickWindow* window() const;
    void setWindow( QQuickWindow* );

    void setEasingCurve( QEasingCurve::Type type );
    void setEasingCurve( const QEasingCurve& );

    const QEasingCurve& easingCurve() const;

    void setAutoRepeat( bool );
    bool autoRepeat() const;

    void setDuration( int ms );
    int duration() const;

    bool isRunning() const;
    qint64 elapsed() const;

    void start();
    void stop();
    void update();

    static QMetaObject::Connection addCleanupHandler(
        QObject* receiver, const char* method,
        Qt::ConnectionType type = Qt::AutoConnection );

    static QMetaObject::Connection addAdvanceHandler(
        QObject* receiver, const char* method,
        Qt::ConnectionType type = Qt::AutoConnection );

#ifndef QT_NO_DEBUG_STREAM
    static void debugStatistics( QDebug );
#endif

  protected:
    virtual void setup();
    virtual void advance( qreal value ) = 0;
    virtual void done();

  private:
    QQuickWindow* m_window;

    int m_duration;
    QEasingCurve m_easingCurve;
    qint64 m_startTime; // quint32 might be enough

    bool m_autoRepeat = false;
};

inline bool QskAnimator::isRunning() const
{
    return m_startTime >= 0;
}

inline int QskAnimator::duration() const
{
    return m_duration;
}

inline bool QskAnimator::autoRepeat() const
{
    return m_autoRepeat;
}

#endif
