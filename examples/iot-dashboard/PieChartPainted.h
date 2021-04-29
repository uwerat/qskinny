/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef PIECHARTPAINTED_H
#define PIECHARTPAINTED_H

#include <QskControl.h>

#include "CircularProgressBar.h"

class ProgressBarAnimator;

class QskTextLabel;
class QQuickPaintedItem;

class PieChartPainted : public QskControl
{
    public:
        QSK_SUBCONTROLS( Panel )

        PieChartPainted( const QColor& color, const QskGradient& gradient, int progress, int value, QQuickItem* parent = nullptr );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final;

        virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;
        void updateLayout() override;

    private:
        QColor m_color;
        QskGradient m_gradient;
        CircularProgressBar* m_progressBar;
        QskTextLabel* m_progressLabel;
        ProgressBarAnimator* m_animator;
};

#endif // PIECHARTPAINTED_H
