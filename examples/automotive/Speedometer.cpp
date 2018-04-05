#include "Speedometer.h"

#include <QskSkinnable.h>
#include <QskSkinlet.h>

QSK_SUBCONTROL( Speedometer, Panel )
QSK_SUBCONTROL( Speedometer, Ticks )
QSK_SUBCONTROL( Speedometer, Numbers )
QSK_SUBCONTROL( Speedometer, Needle )

Speedometer::Speedometer( QQuickItem* parent ) :
    QskControl( parent ),
    m_value( 0.0 )
{
}

QSizeF Speedometer::contentsSizeHint() const
{
    return { 300, 300 };
}

float Speedometer::value() const
{
    return m_value;
}

void Speedometer::setValue( float value )
{
    m_value = value;
    update();
}

#include "moc_Speedometer.cpp"
