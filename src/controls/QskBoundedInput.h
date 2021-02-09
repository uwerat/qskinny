/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOUNDED_INPUT_H
#define QSK_BOUNDED_INPUT_H

#include "QskBoundedControl.h"

class QskIntervalF;

class QSK_EXPORT QskBoundedInput : public QskBoundedControl
{
    Q_OBJECT

    Q_PROPERTY( qreal stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged )
    Q_PROPERTY( int pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged )

    Q_PROPERTY( bool snap READ snap WRITE setSnap NOTIFY snapChanged )
    Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged )

    using Inherited = QskBoundedControl;

  public:
    QSK_STATES( ReadOnly )

    QskBoundedInput( QQuickItem* parent = nullptr );
    ~QskBoundedInput() override;

    qreal stepSize() const;
    int pageSize() const;

    void setSnap( bool );
    bool snap() const;

    void setReadOnly( bool );
    bool isReadOnly() const;

  public Q_SLOTS:
    void setStepSize( qreal );
    void setPageSize( int );

    void stepUp();
    void stepDown();
    void pageUp();
    void pageDown();

    virtual void increment( qreal offset ) = 0;

  Q_SIGNALS:
    void stepSizeChanged( qreal );
    void pageSizeChanged( qreal );
    void snapChanged( bool );

    void readOnlyChanged( bool );

  protected:
    void keyPressEvent( QKeyEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
#endif

    void componentComplete() override;
    virtual void alignInput();

    qreal alignedValue( qreal ) const;
    QskIntervalF alignedInterval( const QskIntervalF& ) const;

  private:
    qreal m_stepSize = 0.1;
    int m_pageSize = 1;
    bool m_snap = false;
};

#endif
