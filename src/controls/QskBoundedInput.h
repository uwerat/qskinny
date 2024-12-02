/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOUNDED_INPUT_H
#define QSK_BOUNDED_INPUT_H

#include "QskBoundedControl.h"

class QskIntervalF;

class QSK_EXPORT QskBoundedInput : public QskBoundedControl
{
    Q_OBJECT

    Q_PROPERTY( qreal stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged )
    Q_PROPERTY( uint pageSteps READ pageSteps WRITE setPageSteps NOTIFY pageStepsChanged )

    Q_PROPERTY( bool snapping READ isSnapping WRITE setSnapping NOTIFY snappingChanged )
    Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged )

    using Inherited = QskBoundedControl;

  public:
    QSK_STATES( ReadOnly )

    QskBoundedInput( QQuickItem* parent = nullptr );
    ~QskBoundedInput() override;

    qreal stepSize() const;
    qreal pageSize() const; // pageSteps() * stepSize()
    uint pageSteps() const;

    void setSnapping( bool );
    bool isSnapping() const;

    void setReadOnly( bool );
    bool isReadOnly() const;

  public Q_SLOTS:
    void setStepSize( qreal );
    void setPageSteps( uint );

    void stepUp();
    void stepDown();
    void pageUp();
    void pageDown();

    virtual void increment( qreal offset ) = 0;

  Q_SIGNALS:
    void stepSizeChanged( qreal );
    void pageStepsChanged( qreal );
    void snappingChanged( bool );

    void readOnlyChanged( bool );

  protected:
    void keyPressEvent( QKeyEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
#endif

    void componentComplete() override;
    virtual void alignInput();

    qreal incrementForKey( const QKeyEvent* ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
