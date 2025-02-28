/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SLIDER_H
#define QSK_SLIDER_H

#include "QskBoundedValueInput.h"
#include "QskNamespace.h"

class QSK_EXPORT QskSlider : public QskBoundedValueInput
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( bool inverted READ isInverted
        WRITE setInverted NOTIFY invertedChanged )

    Q_PROPERTY( bool tracking READ isTracking
        WRITE setTracking NOTIFY trackingChanged )

    Q_PROPERTY( Qsk::Policy tickPolicy READ tickPolicy
        WRITE setTickPolicy RESET resetTickPolicy NOTIFY tickPolicyChanged )

    Q_PROPERTY( qreal origin READ origin
        WRITE setOrigin RESET resetOrigin NOTIFY originChanged )

    using Inherited = QskBoundedValueInput;

  public:
    QSK_SUBCONTROLS( Panel, Groove, Fill, Scale, Tick, Handle )
    QSK_STATES( Pressed )

    explicit QskSlider( QQuickItem* parent = nullptr );
    explicit QskSlider( Qt::Orientation, QQuickItem* parent = nullptr );

    ~QskSlider() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setTickPolicy( Qsk::Policy );
    void resetTickPolicy();
    Qsk::Policy tickPolicy() const;

    void setInverted( bool );
    bool isInverted() const;

    void resetOrigin();
    qreal origin() const;
    bool hasOrigin() const;

    void setTracking( bool );
    bool isTracking() const;

    qreal handlePosition() const; // [0,0, 1.0]

    QskAspect::Variation effectiveVariation() const override;

  public Q_SLOTS:
    void setOrigin( qreal );

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void invertedChanged( bool );
    void trackingChanged( bool );
    void originChanged( qreal );
    void tickPolicyChanged( Qsk::Policy );

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;

    void aboutToShow() override;
    void componentComplete() override;

  private:
    void moveHandle();
    void moveHandleTo( qreal value, const QskAnimationHint& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
