/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Plot.h"
#include "PlotSkin.h"
#include "PlotCursor.h"

#include <QskPlotGrid.h>
#include <QskPlotCorridor.h>
#include <QskPlotCorridorData.h>
#include <QskPlotCurve.h>
#include <QskPlotCurveData.h>
#include <QskEvent.h>

#include <QskMargins.h>
#include <QskRgbValue.h>

#include <QTime>
#include <cmath>

namespace
{
    class CurveData : public QskPlotCurveData
    {
      public:
        CurveData( QObject* parent = nullptr )
            : QskPlotCurveData( parent )
        {
            setHint( MonotonicX );
        }

        void setSamples( const QVector< Plot::Sample >& samples )
        {
            m_samples = samples;
            Q_EMIT changed();
        }

        qsizetype count() const override
        {
            return m_samples.count();
        }

        QPointF pointAt( qsizetype index ) const override
        {
            const auto& sample = m_samples.at( index );
            return QPointF( sample.timestamp, sample.value );
        }

      private:
        QVector< Plot::Sample > m_samples;
    };

    class CorridorData : public QskPlotCorridorData
    {
      public:
        CorridorData( QObject* parent = nullptr )
            : QskPlotCorridorData( parent )
        {
        }

        void setSamples( const QVector< Plot::Sample >& samples )
        {
            m_samples = samples;
            Q_EMIT changed();
        }

        qsizetype count() const override
        {
            return m_samples.count();
        }

        QskPlotCorridorSample sampleAt( qsizetype index ) const override
        {
            const auto& sample = m_samples.at( index );
            return { sample.timestamp, { sample.lowerBound, sample.upperBound } };
        }

      private:
        QVector< Plot::Sample > m_samples;
    };
}

class Plot::PrivateData
{
  public:
    QskPlotGrid* grid = nullptr;
    QskPlotCorridor* corridor = nullptr;
    QskPlotCurve* curve = nullptr;
    PlotCursor* cursor = nullptr;

    const QTime startTime = QTime::currentTime();
};

Plot::Plot( QQuickItem* parentItem )
    : QskPlotView( parentItem )
    , m_data( new PrivateData )
{
    PlotSkin::extendSkin( effectiveSkin() );

    setAcceptedMouseButtons( Qt::LeftButton ); // cursor
    setWheelEnabled( true ); // zooming

    resetAxes();

    using namespace QskRgb;

    m_data->grid = new QskPlotGrid( this );

    m_data->corridor = new QskPlotCorridor( this );
    m_data->corridor->setData( new CorridorData() ); 

    m_data->curve = new QskPlotCurve( this );
    m_data->curve->setData( new CurveData() );

    m_data->cursor = new PlotCursor();
    m_data->cursor->setParent( this ); // not attached
    m_data->cursor->setPosition( -33 );

    /*
        extra space for the overlapping labels. Actually this should be
        the job of the layout code:  TODO ...
     */
    setPaddingHint( Panel, QskMargins( 10, 15, 40, 10 ) );
}

Plot::~Plot()
{
}

void Plot::setSamples( const QVector< Sample >& samples )
{
    auto corridorData = static_cast< CorridorData* >( m_data->corridor->data() );
    corridorData->setSamples( samples );

    auto curveData = static_cast< CurveData* >( m_data->curve->data() );
    curveData->setSamples( samples );
}

void Plot::shiftXAxis( int steps )
{
    auto range = boundaries( QskPlot::XBottom );
    //range.translate( steps * 0.2 * range.width() );
    range.translate( steps * 1.0 );
    setBoundaries( QskPlot::XBottom, range );
}

void Plot::resetAxes()
{
    QskIntervalF rangeX( -50.0, 0.0 );

    if ( m_data->curve && m_data->curve->data() )
    {
        const auto r = m_data->curve->data()->boundingRect();
        if ( !r.isEmpty() )
            rangeX |= QskIntervalF( r.left(), r.right() );
    }

    setBoundaries( QskPlot::XBottom, rangeX );
    setBoundaries( QskPlot::YLeft, 0.0, 100.0 );
}

QVariant Plot::labelAt( QskPlot::Axis axis, qreal pos ) const
{
    if ( axis == QskPlot::XBottom )
    {
        auto text = QString::number( pos, 'g' );
        text += '\n';

        const auto time = m_data->startTime.addSecs( qRound( pos ) );
        text += time.toString();

        return text;
    }

    return Inherited::labelAt( axis, pos );
}

void Plot::mousePressEvent( QMouseEvent* event )
{
    auto pos = qskMousePosition( event );
    if ( canvasRect().contains( pos ) )
    {
        m_data->cursor->attach( this );
        m_data->cursor->setCanvasPosition( pos.x() );

        return;
    }

    Inherited::mousePressEvent( event );
}

void Plot::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_data->cursor->view() )
    {
        auto x = qskMousePosition( event ).x();

        const auto r = canvasRect();
        x = qBound( r.left(), x, r.right() );

        m_data->cursor->setCanvasPosition( x );

        return;
    }

    Inherited::mouseMoveEvent( event );
}

void Plot::mouseReleaseEvent( QMouseEvent* event )
{
    if ( m_data->cursor->view() )
    {
        m_data->cursor->detach();
        return;
    }

    Inherited::mouseReleaseEvent( event );
}

void Plot::wheelEvent( QWheelEvent* event )
{
    const auto steps = qskWheelSteps( event );

    double f = std::pow( 0.9, qAbs( steps ) );
    if ( steps > 0 )
        f = 1 / f;

    auto range = boundaries( QskPlot::XBottom );
    range.setLowerBound( range.upperBound() - f * range.length() );

    setBoundaries( QskPlot::XBottom, range );
}

void Plot::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::StyleChange )
        PlotSkin::extendSkin( effectiveSkin() );

    Inherited::changeEvent( event );
}

#include "moc_Plot.cpp"
