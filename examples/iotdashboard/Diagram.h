/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class Diagram : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Chart, Segments,
                     ChartLine1, ChartArea1, ChartLine2,
                     ChartArea2, ChartLine3, ChartArea3,
                     ChartBar1, ChartBar2, ChartBar3 )

    enum Type
    {
        Line = 0x01,
        Area = 0x02,
        Bar = 0x04,
    };

    Q_DECLARE_FLAGS( Types, Type )

    Diagram( QQuickItem* parent = nullptr );
    ~Diagram() override;

    QVector< QVector< QPointF > > dataPoints() const;
    void addDataPoints( const QVector< QPointF >&, const Types& );

    Types typesAt( uint pos ) const;

    qreal yMax() const;
    void setYMax( qreal yMax );

    int xGridLines() const;
    void setXGridLines( int lines );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( Diagram::Types )
