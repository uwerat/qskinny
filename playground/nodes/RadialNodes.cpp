#include "RadialNodes.h"

QSK_SUBCONTROL(RadialTickmarks, Lines)

RadialTickmarks::RadialTickmarks( QQuickItem* const parent ) : QskControl( parent )
{
    // TODO move into your skin
    setColor(Lines, Qt::red);
    setStrutSizeHint( Lines, 2, 10 );
}

#include "moc_RadialNodes.cpp"