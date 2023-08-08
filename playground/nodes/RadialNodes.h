#pragma once

#include <QskControl.h>

class RadialTickmarks : public QskControl
{
public:
    QSK_SUBCONTROLS(Lines)
    explicit RadialTickmarks( QQuickItem* parent = nullptr );
};