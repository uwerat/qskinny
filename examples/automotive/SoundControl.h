#ifndef SOUNDCONTROL_H
#define SOUNDCONTROL_H

#include <QskBox.h>

class SoundControl : public QskBox
{
public:
    QSK_SUBCONTROLS( Overlay, CrossHair, Marker, Vehicle, SliderControl, MarkerControl )

    SoundControl( QQuickItem* parent = nullptr );

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override final;
};

#endif
