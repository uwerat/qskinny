/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include "CircularProgressBar.h"

class ProgressBarAnimator;
class QskTextLabel;

class PieChartPainted : public QskControl
{
  public:
    QSK_SUBCONTROLS( Panel )

    PieChartPainted( const QColor&, const QskGradient&,
        int progress, int value, QQuickItem* parent = nullptr );

  protected:
    virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;
    void updateLayout() override;

  private:
    QColor m_color;
    QskGradient m_gradient;
    CircularProgressBar* m_progressBar;
    QskTextLabel* m_progressLabel;
    ProgressBarAnimator* m_animator;
};
