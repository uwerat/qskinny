#ifndef SPEEDOMETERDISPLAY_H
#define SPEEDOMETERDISPLAY_H

#include <QskControl.h>

class QskLinearBox;
class QskTextLabel;
class Speedometer;

class SpeedometerDisplay : public QskControl
{
public:
    SpeedometerDisplay( QQuickItem* parent = nullptr );

protected:
    void updateLayout() override;

private:
    QskLinearBox* m_box;
    Speedometer* m_revCounter;
    QskTextLabel* m_revCounterText;
    Speedometer* m_speedometer;
    QskTextLabel* m_speedometerText;
    Speedometer* m_fuelGauge;
    QskTextLabel* m_fuelGaugeText;
};

#endif // SPEEDOMETERDISPLAY_H
