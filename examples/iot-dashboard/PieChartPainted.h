#ifndef PIECHARTPAINTED_H
#define PIECHARTPAINTED_H

#include <QskControl.h>

#include "CircularProgressBar.h"

class QskTextLabel;
class QQuickPaintedItem;

class PieChartPainted : public QskControl
{
public:
    PieChartPainted(const QColor& color, const QGradient& gradient, int progress, int value, QQuickItem* parent = nullptr);

    virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;
    void updateLayout() override;

private:
    QColor m_color;
    QGradient m_gradient;
    CircularProgressBar* m_progressBar;
    QskTextLabel* m_progressLabel;
//    QskTextLabel* m_numberLabel;
//    QskTextLabel* m_unitLabel;
};

#endif // PIECHARTPAINTED_H
