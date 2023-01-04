#pragma once

#include "EnergyMeter.h"
#include <QskGradient.h>

class StorageMeter final : public EnergyMeter
{
  public:
    QSK_SUBCONTROLS( Fill )
    explicit StorageMeter(QQuickItem* parent = nullptr) noexcept;
    qreal progress() const noexcept;
  public Q_SLOTS:
    void setProgress(qreal progress) noexcept;
};
