#include "Speedometer.h"

#include <QskSkinnable.h>
#include <QskSkinlet.h>

QSK_SUBCONTROL( Speedometer, Panel )
QSK_SUBCONTROL( Speedometer, Ticks )
QSK_SUBCONTROL( Speedometer, Numbers )
QSK_SUBCONTROL( Speedometer, Needle )

Speedometer::Speedometer( QQuickItem* parent ) :
    QskControl( parent )
{
}

QSizeF Speedometer::contentsSizeHint() const
{
    return QSizeF( 300, 300 );
}

#include "moc_Speedometer.cpp"
