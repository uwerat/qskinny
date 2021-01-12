/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SLIDER_H
#define QSK_SLIDER_H

#include "QskBoundedValueInput.h"

class QSK_EXPORT QskSlider : public QskBoundedValueInput
{
    Q_OBJECT

    Q_PROPERTY( bool isPressed READ isPressed NOTIFY pressedChanged )

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( bool tracking READ isTracking WRITE setTracking NOTIFY trackingChanged )
    Q_PROPERTY( qreal handlePosition READ handlePosition )

    using Inherited = QskBoundedValueInput;

  public:
    QSK_SUBCONTROLS( Panel, Groove, Fill, Scale, Handle )
    QSK_STATES( Pressed, Minimum, Maximum )

    explicit QskSlider( QQuickItem* parent = nullptr );
    explicit QskSlider( Qt::Orientation, QQuickItem* parent = nullptr );

    ~QskSlider() override;

    bool isPressed() const;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setTracking( bool );
    bool isTracking() const;

    qreal handlePosition() const; // [0,0, 1.0]

    QskAspect::Placement effectivePlacement() const override;

  Q_SIGNALS:
    void pressedChanged( bool );
    void orientationChanged( Qt::Orientation );
    void trackingChanged( bool );

  protected:
    void mousePressEvent( QMouseEvent* e ) override;
    void mouseMoveEvent( QMouseEvent* e ) override;
    void mouseReleaseEvent( QMouseEvent* e ) override;

    QSizeF handleSize() const;
    QRectF handleRect() const;

    void aboutToShow() override;

  private:
    void moveHandle();
    void moveHandleTo( qreal value, const QskAnimationHint& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
