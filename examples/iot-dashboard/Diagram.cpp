#include "Diagram.h"

#include <QVectorIterator>

class Diagram::PrivateData
{
    public:
        PrivateData()
        {
        }

        QVector<QPointF> dataPoints;
        int xGridLines = -1;
        qreal yMax = -1;
        Qsk::Position position = Qsk::Bottom;
        Types types = Area;
};

QSK_SUBCONTROL( Diagram, Chart )
QSK_SUBCONTROL( Diagram, Segments )
QSK_SUBCONTROL( Diagram, ChartLine )
QSK_SUBCONTROL( Diagram, ChartArea )

Diagram::Diagram( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
}

Diagram::~Diagram()
{
}

QVector<QPointF> Diagram::dataPoints() const
{
    return m_data->dataPoints;
}

void Diagram::setDataPoints( const QVector<QPointF>& dataPoints )
{
    m_data->dataPoints = dataPoints;
}

Diagram::Types Diagram::types() const
{
    return m_data->types;
}

void Diagram::setTypes( Types types )
{
    m_data->types = types;
}

qreal Diagram::yMax() const
{
    return m_data->yMax;
}

void Diagram::setYMax( qreal yMax )
{
    m_data->yMax = yMax;
}

int Diagram::xGridLines() const
{
    return m_data->xGridLines;
}

void Diagram::setXGridLines( int lines )
{
    m_data->xGridLines = lines;
}

Qsk::Position Diagram::chartPosition() const
{
    return m_data->position;
}

void Diagram::setChartPosition( Qsk::Position position )
{
    m_data->position = position;
}

QSizeF Diagram::contentsSizeHint( Qt::SizeHint which, const QSizeF& ) const
{
    if( which != Qt::PreferredSize )
    {
        return QSizeF();
    }

    return {};
}

#include "moc_Diagram.cpp"
