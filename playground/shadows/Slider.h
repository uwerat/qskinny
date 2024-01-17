/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class QskSlider;
class QskTextLabel;

class Slider : public QskLinearBox
{
    Q_OBJECT

    using Inherited = QskLinearBox;

  public:
    Slider( const QString&, qreal min, qreal max, qreal step,
        qreal value, QQuickItem* parent = nullptr );

    qreal value() const;

  Q_SIGNALS:
    void valueChanged( qreal );

  public Q_SLOTS:
    void setValue( qreal );

  private:
    void updateLabel( qreal );

    QskTextLabel* m_label = nullptr;
    QskSlider* m_slider = nullptr;
    QskTextLabel* m_valueLabel = nullptr;
};
