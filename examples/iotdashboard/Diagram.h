/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include <QskNamespace.h>

class Diagram : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Chart, Segments, ChartLine1, ChartArea1,
        ChartLine2, ChartArea2, ChartLine3, ChartArea3 )

    enum Type
    {
        Line = 0x01,
        Area = 0x02,
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

    Qsk::Position chartPosition() const;
    void setChartPosition( Qsk::Position );

  protected:
    QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( Diagram::Types )
