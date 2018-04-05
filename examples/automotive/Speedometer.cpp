#include "Speedometer.h"

#include <QskSkinlet.h>
#include <QskSkinnable.h>

QSK_SUBCONTROL( Speedometer, Panel )
QSK_SUBCONTROL( Speedometer, Labels )
QSK_SUBCONTROL( Speedometer, Needle )

Speedometer::Speedometer( QQuickItem* parent ) :
    QskControl( parent ),
    m_value( 0.0 ),
    m_startAngle( -215 ),
    m_endAngle( 35 )
{
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

float Speedometer::startAngle() const
{
    return m_startAngle;
}

void Speedometer::setStartAngle( float startAngle )
{
    m_startAngle = startAngle;
}

float Speedometer::endAngle() const
{
    return m_endAngle;
}

void Speedometer::setEndAngle( float endAngle )
{
    m_endAngle = endAngle;
}

QVector< QString > Speedometer::labels() const
{
    return m_labels;
}

void Speedometer::setLabels( const QVector< QString >& labels )
{
    m_labels = labels;
}

#include "moc_Speedometer.cpp"
