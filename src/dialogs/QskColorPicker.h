/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_COLOR_PICKER_H
#define QSK_COLOR_PICKER_H

#include "QskBoundedControl.h"

#include <memory>

class QskColorPicker : public QskBoundedControl
{
    Q_OBJECT

    using Inherited = QskBoundedControl;

  public:
    QSK_SUBCONTROLS( Panel, ColorPane, Selector )

    QskColorPicker( QQuickItem* parentItem = nullptr );
    ~QskColorPicker() override;

    QColor selectedColor() const;

    qreal value() const; // value as in hue / saturation / value
    qreal valueAsRatio() const; // [0.0, 1.0]
    using QskBoundedControl::valueAsRatio;

    QImage image() const;
    QPointF position() const;

  public Q_SLOTS:
    void setValue( qreal );
    void setValueAsRatio( qreal );

  Q_SIGNALS:
    void valueChanged( qreal );
    void selectedColorChanged();
    void positionChanged();

  protected:
    void updateLayout() override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

  private:
    void updatePosition( const QPointF& );
    void createImage();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
