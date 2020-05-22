#include "PieChart.h"

QSK_SUBCONTROL( PieChart, Panel )
QSK_SUBCONTROL( PieChart, Labels )

PieChart::PieChart( QQuickItem *parent ) : QskControl( parent )
{
}

QVector< float > PieChart::angles() const
{
    return m_angles;
}

void PieChart::setAngles( const QVector< float >& angles )
{
    m_angles = angles;
}

QVector< QString > PieChart::labels() const
{
    return m_labels;
}

void PieChart::setLabels( const QVector< QString >& labels )
{
    m_labels = labels;
}
