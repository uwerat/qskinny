/******************************************************************************
 * QSkinny - Copyright (C) The authors
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ChartView.h"
#include "ChartSample.h"
#include "CircularChart.h"

#include <QskArcMetrics.h>
#include <QskGraphic.h>
#include <QskRgbValue.h>

#include <QskBox.h>
#include <QskLinearBox.h>
#include <QskGridBox.h>
#include <QskTextLabel.h>
#include <QskGraphicLabel.h>
#include <QskSlider.h>

#include <qpainter.h>

namespace
{
    class ChartBox : public QskControl
    {
        Q_OBJECT

      public:
        ChartBox( CircularChart* chart, QQuickItem* parent = nullptr )
            : QskControl( parent )
            , m_chart( chart )
        {
            m_chart->setParentItem( this );
            if ( m_chart->parent() == nullptr )
                m_chart->setParent( this );

            setPolishOnResize( true );
            setMargins( 10 );
        }

        QskArcMetrics arcMetrics() const
        {
            return m_chart->arcMetrics();
        }

      public Q_SLOTS:
        void setThickness( qreal thickness )
        {
            m_chart->setArcThickness( thickness, Qt::RelativeSize );
        }

        void setStartAngle( qreal degrees )
        {
            auto metrics = m_chart->arcMetrics();
            metrics.setStartAngle( degrees );
            m_chart->setArcMetrics( metrics );
        }

        void setSpanAngle( qreal degrees )
        {
            auto metrics = m_chart->arcMetrics();
            metrics.setSpanAngle( degrees );
            m_chart->setArcMetrics( metrics );

            polish();
        }

      protected:
        void updateLayout() override
        {
            const auto r = layoutRect();

            m_chart->setArcDiameters( r.size() );

            const auto align = Qt::AlignTop | Qt::AlignHCenter;
            m_chart->setGeometry( qskAlignedRectF( r, m_chart->sizeHint(), align ) );
        }

      private:
        CircularChart* m_chart = nullptr;
    };
}

namespace
{
    class SliderBox : public QskLinearBox
    {
        Q_OBJECT

      public:
        SliderBox( const QString& label, qreal min, qreal max, qreal value )
        {
            auto textLabel = new QskTextLabel( label, this );
            textLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

            auto slider = new QskSlider( this );
            slider->setBoundaries( min, max );
            slider->setValue( value );
            slider->setStepSize( 1.0 );
            slider->setPageSize( 10.0 );

            connect( slider, &QskSlider::valueChanged,
                this, &SliderBox::valueChanged );
        }

      Q_SIGNALS:
        void valueChanged( qreal );
    };
}

namespace
{
    class ControlPanel : public QskGridBox
    {
        Q_OBJECT

      public:
        ControlPanel( const QskArcMetrics& metrics, QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setPanel( true );
            setPaddingHint( QskBox::Panel, 20 );
            setSpacing( 10 );

            auto sliderStart = new SliderBox( "Angle", 0.0, 360.0, metrics.startAngle() );
            auto sliderSpan = new SliderBox( "Span", -360.0, 360.0, metrics.spanAngle() );
            auto sliderExtent = new SliderBox( "Extent", 10.0, 100.0, metrics.thickness() );

            connect( sliderStart, &SliderBox::valueChanged,
                this, &ControlPanel::startAngleChanged );

            connect( sliderSpan, &SliderBox::valueChanged,
                this, &ControlPanel::spanAngleChanged );

            connect( sliderExtent, &SliderBox::valueChanged,
                this, &ControlPanel::thicknessChanged );

            addItem( sliderStart, 0, 0 );
            addItem( sliderExtent, 0, 1 );
            addItem( sliderSpan, 1, 0, 1, 2 );
        }

      Q_SIGNALS:
        void thicknessChanged( qreal );
        void startAngleChanged( qreal );
        void spanAngleChanged( qreal );
    };

    class Legend : public QskGridBox
    {
      public:
        Legend( QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setMargins( 10 );
            setLayoutAlignmentHint( Qt::AlignLeft | Qt::AlignTop );
        }

        void setSamples( const QVector< ChartSample >& samples )
        {
            clear( true );

            for ( const auto& sample : samples )
            {
                // using QskBox instead TODO ...
                auto iconLabel = new QskGraphicLabel( graphic( sample.gradient() ) );
                iconLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

                auto textLabel = new QskTextLabel( sample.title() );
                textLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

                const auto row = rowCount();

                addItem( iconLabel, row, 0 );
                addItem( textLabel, row, 1 );
            }
        }

      private:
        QskGraphic graphic( const QskGradient& gradient ) const
        {
            QskGraphic identifier;

            QPainter painter( &identifier );
            painter.setPen( QPen( QskRgb::toTransparent( Qt::black, 100 ), 1 ) );

            QLinearGradient qGradient;
            qGradient.setStops( qskToQGradientStops( gradient.stops() ) );
            painter.setBrush( qGradient );

            painter.drawRect( 0, 0, 20, 20 );
            painter.end();

            return identifier;
        }
    };

}

ChartView::ChartView( CircularChart* chart, QQuickItem* parent )
    : QskMainView( parent )
{
    auto hBox = new QskLinearBox( Qt::Horizontal );

    auto chartBox = new ChartBox( chart, hBox );

    auto legend = new Legend( hBox );
    legend->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    legend->setSamples( chart->series() );

    auto controlPanel = new ControlPanel( chartBox->arcMetrics() );
    controlPanel->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    connect( controlPanel, &ControlPanel::thicknessChanged,
        chartBox, &ChartBox::setThickness );

    connect( controlPanel, &ControlPanel::startAngleChanged,
        chartBox, &ChartBox::setStartAngle );

    connect( controlPanel, &ControlPanel::spanAngleChanged,
        chartBox, &ChartBox::setSpanAngle );

    setHeader( controlPanel );
    setBody( hBox );
}

#include "ChartView.moc"
