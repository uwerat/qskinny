#ifndef BOXWITHBUTTONS_H
#define BOXWITHBUTTONS_H

#include "Box.h"
#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>

class BigRoundedIcon : public RoundedIcon
{
    public:
        QSK_SUBCONTROLS( Panel )

        BigRoundedIcon( const QString& iconName, const QskGradient& gradient, QQuickItem* parent = nullptr )
            : RoundedIcon( iconName, gradient, parent )
        {
            setGradientHint( Panel, gradient );
            setBoxShapeHint( Panel, 6 );
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }
};

class BoxWithButtons : public Box
{
    public:
        QSK_SUBCONTROLS( Panel )

        BoxWithButtons( const QString& title, const QskGradient& gradient, QQuickItem* parent = nullptr );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
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
        IndoorTemperature( QQuickItem* parent )
            : BoxWithButtons( "Indoor Temperature", { QskGradient::Vertical, "#ff7d34", "#ff3122" }, parent )
        {
        }
};

class Humidity: public BoxWithButtons
{
    public:
        Humidity( QQuickItem* parent )
            : BoxWithButtons( "Humidity", { QskGradient::Vertical, "#6776FF", "#6100FF" }, parent )
        {
        }
};

#endif // BOXWITHBUTTONS_H
