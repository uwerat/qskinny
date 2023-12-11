/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
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
#include <QskShadowMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>

#include <qpainter.h>

QSK_SUBCONTROL(ArcControl, Arc)

namespace
{
    class LinearGradientSlider : public QskSlider
    {
        Q_OBJECT
        Q_PROPERTY( QColor selectedColor READ selectedColor NOTIFY selectedColorChanged )
        using Inherited = QskSlider;

      public:
        QSK_STATES( Grayscale, Alpha )

        explicit LinearGradientSlider( QQuickItem* parent = nullptr );
        explicit LinearGradientSlider( Qt::Orientation orientation, QQuickItem* parent = nullptr );
        QColor selectedColor() const;

      protected:
        void mousePressEvent( QMouseEvent* event ) override
        {            
            if ( event->button() == Qt::RightButton )
            {
                // RGB->GRAYSCALE->ALPHA
                const auto a = skinStates().testFlag(Alpha);
                const auto g = skinStates().testFlag(Grayscale);

                if ( g ) 
                {
                    setSkinStateFlag( Grayscale, false );
                    setSkinStateFlag( Alpha, true );
                }
                else if ( a ) 
                {
                    setSkinStateFlag( Grayscale, false );
                    setSkinStateFlag( Alpha, false );
                }
                else 
                {
                    setSkinStateFlag( Grayscale, true );
                    setSkinStateFlag( Alpha, false );
                }
            }
            else 
            {
                QskSlider::mousePressEvent( event );
            }
        }

        void mouseReleaseEvent( QMouseEvent* event ) override
        {
            if ( event->button() == Qt::RightButton ) {}
            else
            {
                QskSlider::mouseReleaseEvent( event );
            }
        }

      Q_SIGNALS:
        void selectedColorChanged();

      private:

        QColor colorForPosition(const QskGradient& gradient, qreal ratio)
        {
            return gradient.extracted( ratio, ratio ).startColor();
        }

        QColor m_color;
    };

    QSK_STATE( LinearGradientSlider, Grayscale, QskAspect::FirstUserState << 0 )
    QSK_STATE( LinearGradientSlider, Alpha, QskAspect::FirstUserState << 1 )

    LinearGradientSlider::LinearGradientSlider( QQuickItem* parent )
        : LinearGradientSlider( Qt::Horizontal, parent )
    {
    }

    LinearGradientSlider::LinearGradientSlider( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( orientation, parent )
    {               
        static const QskGradientStops stopsRGB = {
            { 0.0000, QColor::fromRgb( 255, 0, 0 ) },
            { 0.1667, QColor::fromRgb( 255, 255, 0 ) },
            { 0.3333, QColor::fromRgb( 0, 255, 0 ) },
            { 0.5000, QColor::fromRgb( 0, 255, 255 ) },
            { 0.6667, QColor::fromRgb( 0, 0, 255 ) },
            { 0.8333, QColor::fromRgb( 255, 0, 255 ) },
            { 1.0000, QColor::fromRgb( 255, 0, 0 ) },
        };

        static const QskGradientStops stopsGrayscale = {
            { 0.0000, QColor::fromRgb( 0, 0, 0 ) },
            { 1.0000, QColor::fromRgb( 255, 255, 255 ) },
        };

        static const QskGradientStops stopsAlpha = {
            { 0.0000, QColor::fromRgb( 0, 0, 0, 0) },
            { 1.0000, QColor::fromRgb( 0, 0, 0, 255 ) },
        };

        auto make_gradient = []( const QskGradientStops& stops, Qt::Orientation orientation ) {
            QskGradient gradient(stops);
            gradient.setLinearDirection(orientation);
            return gradient;
        };

        static constexpr auto Horizontal = static_cast<QskAspect::Variation>(Qt::Horizontal);
        static constexpr auto Vertical = static_cast<QskAspect::Variation>(Qt::Vertical);

        setColor( Fill, Qt::transparent );
        
        setGradientHint( Groove, make_gradient(stopsRGB, Qt::Horizontal) );
        setGradientHint( Groove | Horizontal, make_gradient(stopsRGB, Qt::Horizontal) );
        setGradientHint( Groove | Vertical, make_gradient(stopsRGB, Qt::Vertical) );

        setGradientHint( Groove | Grayscale, make_gradient( stopsGrayscale, Qt::Horizontal ) );
        setGradientHint( Groove | Grayscale | Horizontal, make_gradient( stopsGrayscale, Qt::Horizontal ) );
        setGradientHint( Groove | Grayscale | Vertical, make_gradient( stopsGrayscale, Qt::Vertical ) );

        setGradientHint( Groove | Alpha, make_gradient( stopsGrayscale, Qt::Horizontal ) );
        setGradientHint( Groove | Alpha | Horizontal, make_gradient( stopsGrayscale, Qt::Horizontal ) );
        setGradientHint( Groove | Alpha | Vertical, make_gradient( stopsGrayscale, Qt::Vertical ) );
     
        setBoxBorderColorsHint( Handle, Qt::white );
        setBoxBorderMetricsHint( Handle, 2 );        

        connect( this, &QskSlider::valueChanged, this, [ this ]( qreal value ) {
            value = this->orientation() == Qt::Horizontal ? value : 1.0 - value;
            const auto gradient = gradientHint(Groove);
            const auto selectedColor = colorForPosition(gradient, value);
            setColor( Inherited::Handle, selectedColor );
            setColor( Inherited::Ripple, selectedColor );
            setBoxBorderColorsHint( Inherited::Handle, Qt::white );
            if(skinStates().testFlag(Alpha)) 
            {
                m_color.setAlpha(value * 255);
            }
            else 
            {
                m_color.setRed(selectedColor.red());
                m_color.setGreen(selectedColor.green());
                m_color.setBlue(selectedColor.blue());
            }
            Q_EMIT selectedColorChanged();
        } );

        valueChanged(0.0);

        setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    }

    QColor LinearGradientSlider::selectedColor() const
    {
        return m_color;
    }

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
        SliderBox(
            const QString& label, qreal min, qreal max, qreal value,
            std::function< QskSlider*( QQuickItem* ) > allocator =
                []( QQuickItem* parent = nullptr ) { return new QskSlider( parent ); } )
        {
            auto textLabel = new QskTextLabel( label, this );
            textLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

            auto slider = allocator( this );
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
            auto shadowExtent = new SliderBox( "Shadow Extent", 0.0, 1.0, 0.5 );
            auto sliderOffsetX = new SliderBox( "Offset X", -1.0, +1.0, 0 );
            auto sliderOffsetY = new SliderBox( "Offset Y", -1.0, +1.0, 0 );
            auto sliderStrokeWidth = new SliderBox( "Stroke Width", 0, 10, 1 );
            auto sliderFillColor = new SliderBox( "Fill Color", 0.0, 1.0, 0 , []( QQuickItem* parent = nullptr ) { return new LinearGradientSlider( parent ); });
            auto sliderShadowColor = new SliderBox( "Shadow Color", 0.0, 1.0, 0, []( QQuickItem* parent = nullptr ) { return new LinearGradientSlider( parent ); } );
            auto sliderStrokeColor = new SliderBox( "Stroke Color", 0.0, 1.0, 0, []( QQuickItem* parent = nullptr ) { return new LinearGradientSlider( parent ); } );

            connect( sliderStart, &SliderBox::valueChanged,
                this, &ControlPanel::startAngleChanged );

            connect( sliderSpan, &SliderBox::valueChanged,
                this, &ControlPanel::spanAngleChanged );

            connect( sliderExtent, &SliderBox::valueChanged,
                this, &ControlPanel::thicknessChanged );

            connect( sliderExtent, &SliderBox::valueChanged,
                this, &ControlPanel::thicknessChanged ); 
            
            connect( sliderOffsetX, &SliderBox::valueChanged,
                this, &ControlPanel::offsetXChanged );

            connect( sliderOffsetY, &SliderBox::valueChanged,
                this, &ControlPanel::offsetYChanged );

            connect( shadowExtent, &SliderBox::valueChanged,
                this, &ControlPanel::shadowExtendChanged );  
            
            connect( sliderStrokeWidth, &SliderBox::valueChanged,
                this, &ControlPanel::strokeWidthChanged );

            connect( sliderFillColor, &SliderBox::valueChanged, this, [=](){
                auto* const slider = sliderFillColor->findChild<LinearGradientSlider*>();
                Q_EMIT fillColorChanged(slider->selectedColor());
            } );

            connect( sliderShadowColor, &SliderBox::valueChanged, this, [=](){
                auto* const slider = sliderShadowColor->findChild<LinearGradientSlider*>();
                Q_EMIT shadowColorChanged(slider->selectedColor());
            } );    
            
            connect( sliderStrokeColor, &SliderBox::valueChanged, this, [=](){
                auto* const slider = sliderStrokeColor->findChild<LinearGradientSlider*>();
                Q_EMIT strokeColorChanged(slider->selectedColor());
            } );

            addItem( sliderStart, 0, 0 );
            addItem( sliderExtent, 0, 1 );
            addItem( shadowExtent, 0, 2 );
            addItem( sliderSpan, 1, 0, 1, 1 );
            addItem( sliderStrokeWidth, 1, 1, 1, 1 );
            addItem( sliderOffsetX, 2, 0, 1, 1 );
            addItem( sliderOffsetY, 2, 1, 1, 1 );
            addItem( sliderFillColor, 3, 0, 1, 1 );
            addItem( sliderShadowColor, 3, 1, 1, 1 );
            addItem( sliderStrokeColor, 3, 2, 1, 1 );
        }

      Q_SIGNALS:
        void thicknessChanged( qreal );
        void startAngleChanged( qreal );
        void spanAngleChanged( qreal );
        void offsetXChanged( qreal );
        void offsetYChanged( qreal );
        void fillColorChanged( QColor );
        void strokeColorChanged( QColor );
        void shadowColorChanged( QColor );
        void shadowExtendChanged( qreal );
        void strokeWidthChanged( qreal );
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

ChartView::ChartView( ArcControl* chart, QQuickItem* parent )
    : QskMainView( parent )
{
    setColor(Panel, Qt::white);

    auto hBox = new QskLinearBox( Qt::Horizontal );

    // auto chartBox = new ChartBox( chart, hBox );

    // auto legend = new Legend( hBox );
    // legend->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    // legend->setSamples( chart->series() );
    hBox->addItem(chart);
    hBox->setDefaultAlignment(Qt::AlignCenter);
    hBox->setMargins(16);

    auto controlPanel = new ControlPanel( chart->arcMetricsHint(QskControl::Background) );
    controlPanel->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );
   
    const auto subcontrol = ArcControl::Arc;

    connect( controlPanel, &ControlPanel::thicknessChanged, chart, [ = ]( qreal v ) {
            auto m = chart->arcMetricsHint( subcontrol );
            m.setThickness( v ) ;
            chart->setArcMetricsHint( subcontrol, m );
        } );

    connect( controlPanel, &ControlPanel::startAngleChanged, chart, [ = ]( qreal v ) {
        auto m = chart->arcMetricsHint( subcontrol );
        m.setStartAngle( v );
        chart->setArcMetricsHint( subcontrol, m );
    } );

    connect( controlPanel, &ControlPanel::spanAngleChanged, chart, [ = ]( qreal v ) {
        auto m = chart->arcMetricsHint( subcontrol );
        m.setSpanAngle( v );
        chart->setArcMetricsHint( subcontrol, m );
    } );

    connect( controlPanel, &ControlPanel::offsetXChanged, chart, [ = ]( qreal v ) {
        auto h = chart->shadowMetricsHint( subcontrol );
        h.setOffsetX( v );
        chart->setShadowMetricsHint( subcontrol, h );
    } );

    connect( controlPanel, &ControlPanel::offsetYChanged, chart, [ = ]( qreal v ) {
        auto h = chart->shadowMetricsHint( subcontrol );
        h.setOffsetY( v );
        chart->setShadowMetricsHint( subcontrol, h );
    } );

    connect( controlPanel, &ControlPanel::shadowExtendChanged, chart, [ = ]( qreal v ) {
        auto h = chart->shadowMetricsHint( subcontrol );
        h.setSpreadRadius( v );
        chart->setShadowMetricsHint( subcontrol, h );
    } );

    connect( controlPanel, &ControlPanel::fillColorChanged, chart,
        [ = ]( QColor c ) { chart->setColor( subcontrol, c ); } );

    connect( controlPanel, &ControlPanel::shadowColorChanged, chart,
        [ = ]( QColor c ) { 
            chart->setShadowColorHint( subcontrol, c ); 
        } );

    connect( controlPanel, &ControlPanel::strokeColorChanged, chart,
        [ = ]( QColor c ) { 
            chart->setColor( subcontrol | QskAspect::Border, c ); 
        } );

    connect( controlPanel, &ControlPanel::strokeWidthChanged, chart, [ = ]( qreal v ) {
        chart->setMetric(subcontrol | QskAspect::Border, v);
    } );

    setHeader( controlPanel );
    setBody( hBox );
}

#include "ChartView.moc"
