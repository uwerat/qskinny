#ifndef SOUNDCONTROL_H
#define SOUNDCONTROL_H

#include <QskControl.h>

class SoundControl : public QskControl
{
public:
    QSK_SUBCONTROLS( CrossHair, Marker, SliderControl )

    SoundControl( QQuickItem* parent = nullptr );
};

#endif // SOUNDCONTROL_H
