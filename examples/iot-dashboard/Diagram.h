#pragma once

#include <QskControl.h>
#include <QskNamespace.h>

class Diagram : public QskControl
{
        Q_OBJECT

        using Inherited = QskControl;

    public:
        QSK_SUBCONTROLS( Chart, Segments, ChartLine, ChartArea )

        enum Type
        {
            Line = 0x01,
            Area = 0x02,
        };

        Q_DECLARE_FLAGS( Types, Type )

        Diagram( QQuickItem* parent = nullptr );
        ~Diagram() override;

        QVector<QPointF> dataPoints() const;
        void setDataPoints( const QVector<QPointF>& dataPoints );

        Types types() const;
        void setTypes( Types types );

        qreal yMax() const;
        void setYMax( qreal yMax );

        int xGridLines() const;
        void setXGridLines( int lines );

        Qsk::Position chartPosition() const;
        void setChartPosition( Qsk::Position position );

    protected:
        QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    private:
        class PrivateData;
        std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( Diagram::Types )
