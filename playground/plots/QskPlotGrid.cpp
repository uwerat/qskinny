/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskPlotGrid.h"
#include "QskPlotView.h"
#include "QskStippleMetrics.h"

#include <QskTickmarks.h>

QSK_SUBCONTROL( QskPlotGrid, MajorLine )
QSK_SUBCONTROL( QskPlotGrid, MinorLine )

static inline QskAspect::Subcontrol qskSubcontrol( QskPlotGrid::Type gridType )
{
    using Q = QskPlotGrid;
    return ( gridType == Q::MinorGrid ) ? Q::MinorLine : Q::MajorLine;
}

class QskPlotGrid::PrivateData
{
};

QskPlotGrid::QskPlotGrid( QObject* object )
    : Inherited( object )
    , m_data( new PrivateData )
{
    setCoordinateType( CanvasCoordinates );
}

QskPlotGrid::~QskPlotGrid()
{
}

void QskPlotGrid::setPen( Type gridType, const QPen& pen )
{
    using A = QskAspect;

    const auto oldPen = this->pen( gridType );

    const auto subControl = qskSubcontrol( gridType );

    setColor( subControl, pen.color() );
    setMetric( subControl | A::Size, pen.widthF() );
    setSkinHint( subControl | A::Metric | A::Style,
        QVariant::fromValue( QskStippleMetrics( pen ) ) );

    if ( oldPen != pen )
    {
        markDirty();

        if ( gridType == MinorGrid )
            Q_EMIT minorPenChanged( pen );
        else
            Q_EMIT majorPenChanged( pen );
    }
}

void QskPlotGrid::resetPen( Type gridType )
{
    using A = QskAspect;

    const auto oldPen = pen( gridType );
    const auto subControl = qskSubcontrol( gridType );

    resetColor( subControl );
    resetMetric( subControl | A::Size );
    resetMetric( subControl | A::Style );

    const auto newPen = pen( gridType );

    if ( oldPen != newPen )
    {
        markDirty();

        if ( gridType == MinorGrid )
            Q_EMIT minorPenChanged( newPen );
        else
            Q_EMIT majorPenChanged( newPen );
    }
}

QPen QskPlotGrid::pen( Type gridType ) const
{
    using A = QskAspect;

    const auto subControl = qskSubcontrol( gridType );

    const auto stippleMetrics = effectiveSkinHint(
        subControl | A::Metric | A::Style ).value< QskStippleMetrics >();

    QPen pen( Qt::NoPen );

    if ( stippleMetrics.isValid() )
    {
        if ( stippleMetrics.isSolid() )
        {
            pen.setStyle( Qt::SolidLine );
        }
        else
        {
            pen.setStyle( Qt::CustomDashLine );
            pen.setDashOffset( stippleMetrics.offset() );
            pen.setDashPattern( stippleMetrics.pattern() );
        }

        pen.setColor( color( subControl ) );
        pen.setWidth( metric( subControl | A::Size ) );
    }

    return pen;
}

QVector< qreal > QskPlotGrid::lines( Type gridType, Qt::Orientation orientation ) const
{
    if ( auto view = this->view() )
    {
        const auto axis = ( orientation == Qt::Horizontal ) ? yAxis() : xAxis();

        const auto& tickmarks = view->tickmarks( axis );

        if ( gridType == MajorGrid )
            return tickmarks.majorTicks();
        else
            return tickmarks.mediumTicks() + tickmarks.minorTicks();
    }

    return QVector< qreal >();
}

bool QskPlotGrid::needsClipping() const
{
    return false;
}

#include "moc_QskPlotGrid.cpp"
