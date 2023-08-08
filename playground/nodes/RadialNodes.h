#pragma once

#include <QskControl.h>
#include <QskScaleTickmarks.h>

class RadialTickmarks : public QskControl
{
public:
    QSK_SUBCONTROLS(Lines)
    explicit RadialTickmarks( QQuickItem* parent = nullptr );
    void setTickmarks(const QskScaleTickmarks& tickmarks);
    QskScaleTickmarks tickmarks() const;

private:
    QskScaleTickmarks m_tickmarks;
};