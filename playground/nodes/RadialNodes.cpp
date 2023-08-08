#include "RadialNodes.h"

QSK_SUBCONTROL(RadialNodes, Foreground)
QSK_SUBCONTROL(RadialNodes, Text)
QSK_SUBCONTROL(RadialNodes, Lines)

RadialNodes::RadialNodes( QQuickItem* const parent ) : QskControl( parent )
{
    // TODO move into your skin
    setColor(Background, Qt::lightGray);
    setColor(Foreground, Qt::black);
    setColor(Text, Qt::white);
    setColor(Lines, Qt::red);
    setStrutSizeHint( Lines, 2, 10 );
}

#include "moc_RadialNodes.cpp"