/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FOCUS_INDICATOR_H
#define QSK_FOCUS_INDICATOR_H

#include "QskControl.h"

class QSK_EXPORT QskFocusIndicator : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

    Q_PROPERTY( int duration READ duration
        WRITE setDuration NOTIFY durationChanged )

  public:
    QSK_SUBCONTROLS( Panel )

    QskFocusIndicator( QQuickItem* parent = nullptr );
    ~QskFocusIndicator() override;

    /*
        duration until the indicator goes into disabled state after being enabled.
        A duration of 0 disables automatic state changes.
     */
    void setDuration( int ms );
    int duration() const;

    bool contains( const QPointF& ) const override;
    QRectF clipRect() const override;

    bool eventFilter( QObject*, QEvent* ) override;

  Q_SIGNALS:
    void durationChanged( int );

  protected:
    void windowChangeEvent( QskWindowChangeEvent* ) override;
    void timerEvent( QTimerEvent* ) override;

    virtual QRectF focusRect() const;
    void updateFocusFrame();

  private:
    void onFocusItemGeometryChanged();
    void onWindowSizeChanged( int );

    void onFocusItemChanged();
    void onFocusItemDestroyed();

    void resetTimer();
    void maybeEnable( bool );

    void connectWindow( const QQuickWindow*, bool on );
    QVector< QMetaObject::Connection > connectItem( const QQuickItem* );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
