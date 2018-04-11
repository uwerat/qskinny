#include "Speedometer.h"

#include <QskSkinlet.h>
#include <QskSkinnable.h>

QSK_SUBCONTROL( Speedometer, Panel )
QSK_SUBCONTROL( Speedometer, Labels )
QSK_SUBCONTROL( Speedometer, Needle )

Speedometer::Speedometer( QQuickItem* parent ) :
    QskRangeControl( parent )
{
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
