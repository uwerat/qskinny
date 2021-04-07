#ifndef BOXWITHBUTTONS_H
#define BOXWITHBUTTONS_H

#include "Box.h"
#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>

class BoxWithButtons : public Box
{
    public:
        QSK_SUBCONTROLS( Panel )

        BoxWithButtons( const QString& title, bool isBright, QQuickItem* parent = nullptr );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }
};

class IndoorTemperature : public BoxWithButtons
{
    public:
        QSK_SUBCONTROLS( Panel )

        IndoorTemperature( QQuickItem* parent )
            : BoxWithButtons( "Indoor Temperature", true, parent )
        {
        }
};

class Humidity: public BoxWithButtons
{
    public:
        QSK_SUBCONTROLS( Panel )

        Humidity( QQuickItem* parent )
            : BoxWithButtons( "Humidity", false, parent )
        {
        }
};

#endif // BOXWITHBUTTONS_H
