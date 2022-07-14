/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "Diagram.h"

QSK_SUBCONTROL( Diagram, Chart )
QSK_SUBCONTROL( Diagram, Segments )
QSK_SUBCONTROL( Diagram, ChartLine1 )
QSK_SUBCONTROL( Diagram, ChartLine2 )
QSK_SUBCONTROL( Diagram, ChartLine3 )
QSK_SUBCONTROL( Diagram, ChartArea1 )
QSK_SUBCONTROL( Diagram, ChartArea2 )
QSK_SUBCONTROL( Diagram, ChartArea3 )
QSK_SUBCONTROL( Diagram, ChartBar1 )
QSK_SUBCONTROL( Diagram, ChartBar2 )
QSK_SUBCONTROL( Diagram, ChartBar3 )

class Diagram::PrivateData
{
  public:
    QVector< QVector< QPointF > > dataPoints;
    int xGridLines = -1;
    qreal yMax = -1;
    QVector< Types > types;
};

Diagram::Diagram( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
}

Diagram::~Diagram()
{
}

QVector< QVector< QPointF > > Diagram::dataPoints() const
{
    return m_data->dataPoints;
}

void Diagram::addDataPoints( const QVector< QPointF >& dataPoints, const Types& types )
{
    m_data->dataPoints.append( dataPoints );
    m_data->types.append( types );
}

Diagram::Types Diagram::typesAt( uint pos ) const
{
    return m_data->types.at( pos );
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

#include "moc_Diagram.cpp"
