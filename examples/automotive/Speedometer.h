#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <QskControl.h>

class Speedometer : public QskControl
{
    Q_OBJECT

public:
    QSK_SUBCONTROLS( Panel, Ticks, Numbers, Needle )

    Speedometer( QQuickItem* parent = nullptr );

    virtual QSizeF contentsSizeHint() const override;
};

#endif // SPEEDOMETER_H
