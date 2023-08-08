#pragma once

#include <QskControl.h>

class RadialNodes : public QskControl
{
public:
    QSK_SUBCONTROLS(Foreground, Text, Lines)
    explicit RadialNodes( QQuickItem* parent = nullptr );
};